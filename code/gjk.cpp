//NOTE this controls how many iterations of GJK we do to get values closer and closer to simplex
//not all these iterations will occur, though. We exit loop when we think we are done
#define GJK_MAX_ITERATION_COUNT 8
//NOTE this basically controls a how close a vector/dist must be for us to consider it zero 
#define GJK_ZERO_EPSILON 0.001f
#define GJK_2D_DEBUG_DRAW_LINES false
#define EPA_2D_DEBUG_DRAW_LINES true

//uncomment this to turn off prints
#pragma push_macro("debug_printf")
//#define debug_printf(...) 

enum Shape_Type {
    SHAPE_CIRCLE,
    SHAPE_AABB,
    SHAPE_RECT,
};

struct Shape {
    Shape_Type type;
    Vector2 pos; //center of shape
    Vector2 dim; //for circle, x is radius
    f32 angle_t;
};

inline bool
is_basically_zero(Vector2 v) {
    return normsq(v) < SQUARED(0.01f); 
}

internal Vector2
get_furthest_vertex_generic(Shape *shape, Vector2 dir) {
    //NOTE we can replace this with a more anaylytic way of finding the extreme vertices
    //but this is more general purpose
    auto find_closest_vertex = [](Vector2 *vertices, s32 vertex_count, Vector2 dir) -> Vector2 {
        assert (vertex_count > 1);
        s32 best_index      = -1; 
        f32 best_dot_product = -F32_MAX;
        for (int i = 0; i < vertex_count; i += 1) {
            f32 dot_product = dot2(dir, vertices[i]);
            if (dot_product > best_dot_product) {
                best_dot_product = dot_product;  
                best_index = i;
            }
        }
        assert (best_index != -1);
        return vertices[best_index]; 
    };
    
    Vector2 result;
    switch(shape->type) {
    case SHAPE_AABB: {
        Vector2 vertices[4];
        f32 half_width  = shape->dim.x*0.5f;
        f32 half_height = shape->dim.y*0.5f;
        
        vertices[0] = v2( half_width,  half_height);
        vertices[1] = v2( half_width, -half_height);
        vertices[2] = v2(-half_width,  half_height);
        vertices[3] = v2(-half_width, -half_height);
        
        result = find_closest_vertex(vertices, countof(vertices), dir);
    } break;
    
    case SHAPE_RECT: {
        Vector2 vertices[4];
        Vector2 half_x_basis = eit2(shape->angle_t);
        Vector2 half_y_basis = perp(half_x_basis);
        half_x_basis *= 0.5f*shape->dim.x;
        half_y_basis *= 0.5f*shape->dim.y;
        
        vertices[0] = ( half_x_basis + half_y_basis);
        vertices[1] = (-half_x_basis + half_y_basis);
        vertices[2] = ( half_x_basis - half_y_basis);
        vertices[3] = (-half_x_basis - half_y_basis);
        
        result = find_closest_vertex(vertices, countof(vertices), dir);
    } break;
    
    case SHAPE_CIRCLE:
    result = normalize(dir)*shape->dim.x;
    break;
    
    /*case COLLIDER_OCTAGON:
    {
        //TODO is there a better more analytic way to compute this?
        Vector2 vertices[8];
        f32 radius = shape->dim.x;
        f32 half_side_length = 0.5f*(0.8284f * radius);
        vertices[0] = v2(-half_side_length,  radius); //top left
        vertices[1] = v2( half_side_length,  radius); //top right
        vertices[2] = v2(-half_side_length, -radius); //bot left
        vertices[3] = v2( half_side_length, -radius); //bot right
        
        vertices[4] = v2(-radius,  half_side_length); //top left
        vertices[5] = v2( radius,  half_side_length); //top right
        vertices[6] = v2(-radius, -half_side_length); //bot left
        vertices[7] = v2( radius, -half_side_length); //bot right
        
        result = find_closest_vertex(vertices, countof(vertices), dir);
    } break; */
    
    default: result = {}; assert (0);
    }
    
    result += shape->pos;
    //if (dot2(dir, sweep) > 0) result += sweep;
    return result;
}

static Vector2
gjk_support(Shape *a, Shape *b, Vector2 dir) { 
    assert (!is_basically_zero(dir));
    Vector2 pa = get_furthest_vertex_generic(a,  dir);//, sweep);   
    Vector2 pb = get_furthest_vertex_generic(b, -dir);
    Vector2 result = pa - pb;
    return result;
}

struct GJK_Simplex_Edge {
    Vector2 dir; //simplex[index_to] - simplex[index_from]
    Vector2 n_to_origin; //vector perp to edge dir facing towards the origin
    Vector2 smallest_delta_to_origin;
    f32 smallest_dist_to_origin;
    f32 dist_to_origin_along_n; //only valid if origin_lies_on_edge_line (meaning it has an a valid normal)
    
    bool origin_lies_on_edge_line;
    bool origin_between_endpoints;
    
    s8 index_from;
    s8 index_to;
    s8 other_index; //if negative then we don't have full simplex yet
};

//NOTE if b is very parallel to a, or b is very small (zero-vector), we return a zero vector
static bool
find_normalized_perp_in_dir(Vector2 line, Vector2 dir, Vector2 *out) {
    f32 signed_area = line.x*dir.y - line.y*dir.x; //area of bivector
    s32 sign = (signed_area < 0) ? -1 : 1;
    
    *out = {};
    if ((signed_area*sign) > GJK_ZERO_EPSILON) {
        *out = {-line.y, line.x};
        *out *= sign;
        
        f32 out_length = norm(*out);
        if (out_length > GJK_ZERO_EPSILON) {
            *out /= out_length;
            return true;
        }
        return false;
    } 
    return false; //lines a parallel
}

struct GJK_Solver_2D {
    Vector2 search_dir; 
    Vector2 simplex[3];
    
    Shape s1;
    Shape s2;
    
    Vector2 debug_draw_pos;
    
    void find_next_point() {
        simplex[2] = simplex[1];
        simplex[1] = simplex[0];
        simplex[0] = gjk_support(&s1, &s2, search_dir);
        
        debug_printf("Added simplex point: (%.3f, %.3f) with search: (%.3f, %.3f)\n", 
                     simplex[0].x, simplex[0].y, search_dir.x, search_dir.y);
    }
    
    void remove_point(s32 index_to_replace) {
        if (index_to_replace == 1) {
            simplex[1] = simplex[2];
        } else if (index_to_replace == 2) {
            //nothing, this will be thrown out on next find_next_point() call
        } else {
            assert (0); //can't remove point 0
        }
    }
    
    GJK_Simplex_Edge make_edge(s8 index_from, s8 index_to) {
        assert (index_from  >= 0 && index_from  < countof(simplex));
        assert (index_to    >= 0 && index_to    < countof(simplex));
        assert (index_from != index_to);
        
        GJK_Simplex_Edge edge = {};
        edge.index_from  = index_from;
        edge.index_to    = index_to;
        edge.other_index = countof(simplex) - (index_from + index_to);
        
        assert (!is_basically_zero(simplex[index_to]));   //otherwise we contain origin already
        assert (!is_basically_zero(simplex[index_from])); //and I failed to detect that and bail
        
        edge.dir = simplex[index_to] - simplex[index_from];
		assert (!is_basically_zero(edge.dir)); //degenerate line case we failed to detect
        
        if (find_normalized_perp_in_dir(edge.dir, -simplex[index_to], &edge.n_to_origin)) {
            f32 d1 = dot2(-edge.dir, -simplex[index_to]);
            if (d1 >= 0) { 
                //NOTE if you imagine the edge line segment spanning along it's perp axis 
                //to make a rectangle infinitely grow on that axis, we're saying the origin lies on that rect
                //specifically on the side facing towards the origin.
                edge.origin_between_endpoints = true;
                edge.dist_to_origin_along_n   = dot2(edge.n_to_origin, -simplex[index_to]);
                edge.smallest_dist_to_origin  = edge.dist_to_origin_along_n;
                edge.smallest_delta_to_origin = edge.n_to_origin*edge.smallest_dist_to_origin;
            } else { //otherwise closest point is at index_to
                edge.dist_to_origin_along_n   = dot2(edge.n_to_origin, -simplex[index_to]);
                edge.smallest_dist_to_origin  = norm(simplex[index_to]);
                edge.smallest_delta_to_origin = -simplex[index_to];
            }
            
            //NOTE I don't expect this case to happen but just to be safe
            //if it's close enough, we say it falls on the edge line
            if (edge.smallest_dist_to_origin < GJK_ZERO_EPSILON) {
                edge.origin_lies_on_edge_line = true;
                edge.dist_to_origin_along_n   = 0;
                edge.smallest_dist_to_origin  = 0;
                edge.smallest_delta_to_origin = {};
            }
            assert (edge.dist_to_origin_along_n >= 0);
        } else {
            //NOTE if we couldn't make a perpendicular direction, then that means the origin is along the edge line
            edge.origin_lies_on_edge_line = true;
            
            f32 edge_length = norm(edge.dir);
            assert (edge_length > GJK_ZERO_EPSILON);
            Vector2 edge_dir_normalized = edge.dir / edge_length;
            assert (!is_basically_zero(edge_dir_normalized));
            
            f32 dist = dot2(edge_dir_normalized, -simplex[index_from]);
            if (dist > edge_length) { //origin continues pass the edge line segment
                edge.smallest_dist_to_origin = dist - edge_length; 
                edge.smallest_delta_to_origin = -simplex[index_to];
            } else if (dist < 0) {
                //NOTE this case really should never happen, since we always pick new points going towards the origin
                edge.smallest_dist_to_origin = -dist; //to make it positive
                edge.smallest_delta_to_origin = -simplex[index_from];
            } else {
                edge.origin_between_endpoints = true;
                edge.smallest_dist_to_origin = 0;
                edge.smallest_delta_to_origin = {};
            }
        }
        
        if_assert (!edge.origin_lies_on_edge_line, !is_basically_zero(edge.n_to_origin)); 
        return edge;
    }
};

static GJK_Solver_2D
make_gjk_solver_2d(Shape *s1, Shape *s2) {
    GJK_Solver_2D solver = {};
    solver.s1 = *s1;
    solver.s2 = *s2;
    
    solver.debug_draw_pos = s2->pos;
    
    //NOTE here we make s2 and s1 pos local to s1 pos, in hopes of improving floating point precision 
    solver.s2.pos -= solver.s1.pos; 
    solver.s1.pos = {};
    
    solver.search_dir = {1, 0}; //it's more stable if this is same value on start
    
    return solver;
}

struct GJK_Result {
    //NOTE for gjk_get_distance_apart(Collider A, a_sweep, B)
    //if (dist_apart <  0), add dist_apart*displacement to move A to NOT overlap with B.
    //if (dist_apart >= 0), add dist_apart*displacement to move A to overlap with B.  
    f32     closest_dist;
    Vector2 closest_delta;
};

//NOTE to maintain convention with GJK, this returns a negative penetration depth
static f32 
epa_get_penetration_depth(GJK_Solver_2D *solver, Vector2 *out_normal) {
    Stack<Vector2, 16> polytope = {};
    for (int i = 0; i < countof(solver->simplex); i += 1) {
        polytope.add(solver->simplex[i]);
    }
    
    auto get_approximate_winding_sign = [](f32 wedge) -> s32 {
        return (wedge < -GJK_ZERO_EPSILON) ? -1 : ((wedge > GJK_ZERO_EPSILON) ? 1 : 0);
    };
    
    Vector2 init_simplex_01 = solver->simplex[1]-solver->simplex[0];
    Vector2 init_simplex_12 = solver->simplex[2]-solver->simplex[1];
    s32 polytope_winding_order = get_approximate_winding_sign(wedge2(init_simplex_01, init_simplex_12));
    assert (polytope_winding_order);
    
    f32 final_dist  = F32_MAX;
    Vector2 final_n = {};
    
    while (polytope.count < polytope.max_count) {
        s32 closest_edge_i0 = -1;
        s32 closest_edge_i1 = -1;
        Vector2 closest_edge_n = {};
        f32 closest_edge_dist = F32_MAX;
        for (int i = 0; i < polytope.count; i += 1) {
            int next_i = (i < (polytope.count-1)) ? (i+1) : 0;
            
            Vector2 edge_dir = polytope[next_i] - polytope[i];
            Vector2 edge_n   = noz(perp(edge_dir), GJK_ZERO_EPSILON);
            assert (!is_basically_zero(edge_n));
            
            f32 edge_dir_wedge_edge_n = wedge2(edge_dir, edge_n);
            s32 sign = get_approximate_winding_sign(edge_dir_wedge_edge_n);
            assert (sign); //would only happend because of precision loss
            
            //if the normal has the same winding order, that means the normal is going inwards (almost like compleing the simplex itself)
            //so we just negate it to get the outwards one
            if (sign == polytope_winding_order) { 
                edge_n *= -1;
            }
            
            f32 edge_dist_to_origin = dot2(edge_n, polytope[i]);
            if (edge_dist_to_origin < closest_edge_dist) {
                closest_edge_dist = edge_dist_to_origin;
                closest_edge_n = edge_n;
                closest_edge_i0 = i;
                closest_edge_i1 = next_i;
            } 
        }
        
        if (closest_edge_i0 == -1) {
            break;
        }   
        
        final_dist = closest_edge_dist;
        final_n = closest_edge_n;
        
        Vector2 new_point = gjk_support(&solver->s1, &solver->s2, closest_edge_n);
        f32 squared_dist_diff = normsq(new_point) - SQUARED(closest_edge_dist);
        if (squared_dist_diff < GJK_ZERO_EPSILON) { 
            //next_point didn't extend farther away from edge, meaning we're very close
            //to the minkowski difference edge. So we just exit here
            break;
        }
        
        
        Vector2 new_edge01 = noz(new_point - polytope[closest_edge_i0], GJK_ZERO_EPSILON);
        Vector2 new_edge12 = noz(polytope[closest_edge_i1] - new_point, GJK_ZERO_EPSILON);
        
        //edges too close that it forms a degenerate line
        if (is_basically_zero(new_edge01) || is_basically_zero(new_edge01)) {
            break;
        }
        
        
        f32 new_edge_area = wedge2(new_edge01, new_edge12);
        s32 new_winding_order = get_approximate_winding_sign(new_edge_area);
        
        if (new_winding_order != polytope_winding_order) {
            //NOTE this means that the new point added forms a degenerate triangle between
            //the other two insertion vertices, in which case we're close enough and bail
            break;
        } else {
            polytope.insert_ordered(new_point, closest_edge_i1);
        }
    }
    
    #if EPA_2D_DEBUG_DRAW_LINES
    for (int i = 0; i < polytope.count; i += 1) {
        int next_i = (i < (polytope.count-1)) ? (i+1) : 0;
            
        Vector2 edge_dir = polytope[next_i] - polytope[i];
        draw_line_delta(solver->debug_draw_pos + polytope[i], edge_dir, 4, V4(1,1,0,1));
    }
    #endif
    
    assert (final_dist != F32_MAX);
    *out_normal = final_n;
    return -final_dist;
}


static GJK_Result
gjk_get_distance_apart(Shape s1, Shape s2) {
    GJK_Solver_2D solver = make_gjk_solver_2d(&s1, &s2);
    solver.find_next_point();
    
    GJK_Result result = {};  
    result.closest_dist = F32_MAX;
    int simplex_contains_origin = 0; //we set to numbers instead of true to know where it was set
    
    if (is_basically_zero(solver.simplex[0])) {
        //got lucky and instantly foind point where simplices overlap
        result.closest_dist = 0;
        goto END_OF_PROCEDURE;
    }
    
    //otherwise continue searching in opposite direction
    result.closest_dist = norm(solver.simplex[0]);
    result.closest_delta = -solver.simplex[0];
    solver.search_dir = -solver.simplex[0];
    debug_printf("Search Dir Changed: (%.3f, %.3f)\n", solver.search_dir.x, solver.search_dir.y); 
    solver.find_next_point();
    
    if (is_basically_zero(solver.simplex[0] - solver.simplex[1])) {
        //degenerate line
        goto END_OF_PROCEDURE;
    } else if (is_basically_zero(solver.simplex[0])) {
        //we got lucky and instantly foind point where simplices overlap
        //but NOTE we have to do some work to find direction
        result = {};
        goto END_OF_PROCEDURE;
    } else { //find new search dir
        GJK_Simplex_Edge edge10 = solver.make_edge(1, 0);
        if (edge10.origin_lies_on_edge_line) { //means we really can't keep searching
            if (edge10.origin_between_endpoints) { //means we should run EPA because there may be closer simplex point samples
                result = {};
                simplex_contains_origin = 1; //run EPA
            } else {
                //NOTE here we assume that closest_delta dir is beyond new point...
                //TODO we may be more exact for extreme, elongated shapes if we try to find a new search dir anyways
                //maybe by trying out both normals of the edge + current_search_dir and see if any lead us to a closer point...
                result.closest_dist = edge10.smallest_dist_to_origin;
                result.closest_delta = -solver.simplex[0];
            }
            goto END_OF_PROCEDURE;
        } else { //we continue searching
            if (edge10.smallest_dist_to_origin < result.closest_dist) {
                result.closest_dist = edge10.smallest_dist_to_origin;
                if (edge10.origin_between_endpoints) {
                    solver.search_dir = edge10.n_to_origin;
                    result.closest_delta = edge10.n_to_origin * result.closest_delta; 
                    debug_printf("Search Dir Changed: (%.3f, %.3f)\n", solver.search_dir.x, solver.search_dir.y);
                } else {
                    solver.search_dir = -solver.simplex[0];
                    result.closest_delta = -solver.simplex[0];
                    debug_printf("Search Dir Changed: (%.3f, %.3f)\n", solver.search_dir.x, solver.search_dir.y);
                }
            } else {
                //NOTE this case should really never happend. I can only imagine it would for floating point imprecision
                //If you imagine a triangle formed by simplex[1] simplex[0] and the origin, it would be a right triangle 
                //and the line from simplex[1] to origin would be the hypotenuse, which is always the longest side of the triangle
                //this case is saying the perp line from simplex[0] and the origin is longer than the hypotenuse ... which if false ... 
                goto END_OF_PROCEDURE; 
            }
        }
    }
    
    Vector4 debug_colors[GJK_MAX_ITERATION_COUNT] = {
        V4(1,0,0,1), V4(0,1,0,1), V4(0,0,1,1), V4(0,1,1,1),
        V4(.15f,0,0,1), V4(0,.15f,0,1), V4(0,0,.15f,1), V4(0,.15f,.15f,1),
    };
    
    #if GJK_2D_DEBUG_DRAW_LINES
    draw_line(s2.pos + solver.simplex[1], s2.pos + solver.simplex[0], 4, V4(0,0,0,1));
    #endif GJK_2D_DEBUG_DRAW_LINES
    
    //from this point on we have a "full simplex", which for 2D means a triangle
    for (int iteration_index = 0; iteration_index < GJK_MAX_ITERATION_COUNT; iteration_index += 1) {
        debug_printf("~~~~~~~~~~GJK Iteration: %d~~~~~~~~~~\n", iteration_index);
        solver.find_next_point();
        #if GJK_2D_DEBUG_DRAW_LINES
        draw_line(s2.pos + solver.simplex[1], s2.pos + solver.simplex[0], 4, debug_colors[iteration_index]);
        #endif
        if (is_basically_zero(solver.simplex[0])) {
            //got lucky
            result = {};
            break;
        }
        
        Vector2 delta_10 = solver.simplex[0] - solver.simplex[1];
        if (is_basically_zero(delta_10)) {
            //degenerate line
            break;
        }
        
        Vector2 delta_20 = solver.simplex[0] - solver.simplex[2];
        if (is_basically_zero(delta_20)) {
            //degenerate line
            break;
        }
        
        //NOTE here we test to see if we have a degenerate triangle case
        f32 collinearity = wedge2(delta_10, delta_20);
        if (absval(collinearity) < GJK_ZERO_EPSILON) { 
            //degenerate triangle
            break;
        }
        
        //NOTE we have to do these checks for edge10 and edge20 because otherwise
        //we wouldn't have a valid n_to_origin for us to use afterwards
        
        GJK_Simplex_Edge edge10 = solver.make_edge(1, 0);
        if (edge10.origin_lies_on_edge_line) {
            if (edge10.origin_between_endpoints) {
                result = {};
                simplex_contains_origin = true; //NOTE we were getting good results without this
            } else if (edge10.smallest_dist_to_origin < result.closest_dist) {
                result.closest_dist  = edge10.smallest_dist_to_origin;
                result.closest_delta = -solver.simplex[0];
            }
            break;
        }
        
        GJK_Simplex_Edge edge20 = solver.make_edge(2, 0);
        if (edge20.origin_lies_on_edge_line) {
            if (edge20.origin_between_endpoints) {
                result = {};
                simplex_contains_origin = true; //NOTE we were getting good results without this
            } else if (edge20.smallest_dist_to_origin < result.closest_dist) {
                result.closest_dist  = edge20.smallest_dist_to_origin;
                result.closest_delta = -solver.simplex[0];
            }
            break;
        }
        
        debug_printf("Edge10: dir=(%.3f, %.3f), n_to_origin=(%.3f, %.3f), dist=%.3f, on_line=%s, between_ends=%s\n",
                     edge10.dir.x, edge10.dir.y, edge10.n_to_origin.x, edge10.n_to_origin.y, edge10.smallest_dist_to_origin, 
                     (edge10.origin_lies_on_edge_line? "true":"false"), (edge10.origin_between_endpoints ? "true":"false"));
        
        debug_printf("Edge20: dir=(%.3f, %.3f), n_to_origin=(%.3f, %.3f), dist=%.3f, on_line=%s, between_ends=%s\n",
                     edge20.dir.x, edge20.dir.y, edge20.n_to_origin.x, edge20.n_to_origin.y, edge20.smallest_dist_to_origin, 
                     (edge20.origin_lies_on_edge_line? "true":"false"), (edge20.origin_between_endpoints ? "true":"false"));
        
        
          //NOTE the first version was better but the .origin_between_endpoints check for both edges will give us false results
          //it turns out for really extreme, elongated shapes, that will cause us to faile
        #if 0
        //NOTE it seems like this test is not all that much better than what we have below...
        //the problem is when points on the simplex get too close together, the dot products returned here
        //can get way too close to zero, where the computations become unstable and we will return we're overlapping when we're not
        //TODO I think the best way to prevent this is just to early exit before the points start getting too close
        //maybe by comparing the collinearity of the next search dir we will set with the previous (if they're too similar just exit out)
        //if (edge10.origin_between_endpoints && edge20.origin_between_endpoints) 
        {
            f32 edge10_n_dot_to_s2 = dot2(edge10.n_to_origin, solver.simplex[2] - solver.simplex[0]);
            f32 edge20_n_dot_to_s1 = dot2(edge20.n_to_origin, solver.simplex[1] - solver.simplex[0]);
            if (edge10_n_dot_to_s2 >= 0 && edge20_n_dot_to_s1 >= 0) {
                debug_printf("!!! Triangle has origin !!!\n");
                debug_printf("With triangle (%.3f,%.3f), (%.3f,%.3f), (%.3f,%.3f): collinearity=%f\n",
                             solver.simplex[2].x, solver.simplex[2].y,
                             solver.simplex[1].x, solver.simplex[1].y,
                             solver.simplex[0].x, solver.simplex[0].y, collinearity);
                simplex_contains_origin = 2;
                result = {};
                break; //run epa
            }    
        } 
        
        #else
        f32 dist_edge10_to_2 = dot2(edge10.n_to_origin, solver.simplex[2] - solver.simplex[0]);
        f32 dist_edge20_to_1 = dot2(edge20.n_to_origin, solver.simplex[1] - solver.simplex[0]);
        
        if (dist_edge10_to_2 > GJK_ZERO_EPSILON && dist_edge20_to_1 > GJK_ZERO_EPSILON) {
            //NOTE and this check tells us we're within the dist bounds we computed beforehand
            if (dist_edge10_to_2 > edge10.dist_to_origin_along_n && 
                dist_edge20_to_1 > edge20.dist_to_origin_along_n) { //this 
                debug_printf("!!! Triangle has origin !!!\n");
                debug_printf("With triangle (%.3f,%.3f), (%.3f,%.3f), (%.3f,%.3f): collinearity=%f\n",
                             solver.simplex[2].x, solver.simplex[2].y,
                             solver.simplex[1].x, solver.simplex[1].y,
                             solver.simplex[0].x, solver.simplex[0].y, collinearity);
                simplex_contains_origin = 2;
                result = {};
                break; //run epa
            }
        }
        #endif
        
        //otherwise we keep searching along the normal of the closest edge
        s32 index_to_replace = 0; //0 will never get replaced so we default to it
        if (edge10.smallest_dist_to_origin < edge20.smallest_dist_to_origin) {
            index_to_replace = 2;
            if (edge10.smallest_dist_to_origin >= result.closest_dist) {
                break; //we're getting farther away
            }
            
            if (edge10.origin_between_endpoints) {
                solver.search_dir = edge10.n_to_origin;
                debug_printf("Search Dir Changed: (%.3f, %.3f)=(%f, %f)\n", solver.search_dir.x, solver.search_dir.y, solver.search_dir.x, solver.search_dir.y);
            } else {
                solver.search_dir = -solver.simplex[0];
                debug_printf("Search Dir Changed: (%.3f, %.3f)=(%f, %f)\n", solver.search_dir.x, solver.search_dir.y, solver.search_dir.x, solver.search_dir.y);
            }
            
            if (edge10.smallest_dist_to_origin < result.closest_dist) {
                f32 dist_diff = result.closest_dist - edge10.smallest_dist_to_origin;
                result.closest_dist  = edge10.smallest_dist_to_origin;
                result.closest_delta = edge10.smallest_delta_to_origin;;
                debug_printf("New closest dist: %.3f->%.3f\n", result.closest_dist, edge10.smallest_dist_to_origin);
                if (dist_diff < 0.05f) {
                    break; //too close
                }
            }
            
        } else { //edge20 closest
            index_to_replace = 1;
            if (edge20.smallest_dist_to_origin >= result.closest_dist) {
                break; //we're getting farther away
            }
            
            //get new search dir
            if (edge20.origin_between_endpoints) {
                solver.search_dir = edge20.n_to_origin;
                debug_printf("Search Dir Changed: (%.3f, %.3f)=(%f, %f)\n", solver.search_dir.x, solver.search_dir.y, solver.search_dir.x, solver.search_dir.y);
            } else {
                solver.search_dir = -solver.simplex[0];
                debug_printf("Search Dir Changed: (%.3f, %.3f)=(%f, %f)\n", solver.search_dir.x, solver.search_dir.y, solver.search_dir.x, solver.search_dir.y);
            }
            
            if (edge20.smallest_dist_to_origin < result.closest_dist) {
                f32 dist_diff = result.closest_dist - edge20.smallest_dist_to_origin;
                result.closest_dist  = edge20.smallest_dist_to_origin;
                result.closest_delta = edge20.smallest_delta_to_origin;
                debug_printf("New closest dist: %.3f->%.3f\n", result.closest_dist, edge20.smallest_dist_to_origin);
                if (dist_diff < 0.05f) {
                    break; //too close
                }    
            }
            
        }
        
        assert (index_to_replace);
        solver.remove_point(index_to_replace);
    }
    
    END_OF_PROCEDURE:;
    if (simplex_contains_origin) {
        result.closest_dist = epa_get_penetration_depth(&solver, &result.closest_delta);
        result.closest_delta *= (-result.closest_dist);
    }
    return result;
         
}



internal void
gjk_draw_minkowski_difference(Vector2 origin, Shape a, Shape b, s32 vertex_count, Vector4 col, f32 thickness = 0.1f) {
    b.pos -= a.pos;
    a.pos = {};
    
    assert (vertex_count > 3);
    //f32  angle_delta = TAU / vertex_count;
    f32 angle_t_delta = 1.0f / (f32)vertex_count;
    Vector2 first_pos = gjk_support(&a, &b, v2(1, 0));
    Vector2 prev_pos  = first_pos;
    for (int i = 1; i < vertex_count; i += 1) {
        Vector2 dir = eit2(angle_t_delta * i);
        Vector2 pos = gjk_support(&a, &b, dir);
        draw_line(origin + prev_pos, origin + pos, thickness, col);
        prev_pos = pos;
        
        if (i == (vertex_count - 1))
            draw_line(origin + pos, origin + first_pos, thickness, col);
    }
}

//NOTE this isn't used for GJK but for finding contact points along a given direction

internal Vector2 
get_furthest_point_generic(Shape shape, Vector2 dir) {
    assert (normsq(dir) > 0);
    normalize(&dir);
    Vector2 result = {};
    switch (shape.type) {
    //TODO don't pass a vector in here, we can do this ourselves with if statements
    case SHAPE_AABB:
	case SHAPE_RECT: {//NOTE hacky, we just rotate result point by the rect angle
        let will_hit_wall = [](f32 dx, f32 wallx, f32 dy, f32 miny, f32 maxy, f32 *tmin) -> nil {
            assert (dx && (maxy > miny));

            nil will_hit = false;
            f32 t = wallx / dx; 
			if(t >= 0) { // && t <= 1.0f) {
                f32 y = dy * t;
                if (y >= miny && y <= maxy) {
                    will_hit = true;
                    *tmin = t;
                }
            }
            return will_hit;
        };
        
        Vector2 pos = {};
        f32 wallx = shape.dim.x*0.5f;
        f32 ceily = shape.dim.y*0.5f;
        if(dir.x) //test vertical walls
        {
            f32 t;
            if(will_hit_wall(dir.x, -wallx, dir.y, -ceily, ceily, &t))
            {
                pos = dir * (t + 0/*distance_away_from_edge*/); 
            }
            else if(will_hit_wall(dir.x, wallx, dir.y, -ceily, ceily, &t))
            {
                pos = dir * (t + 0/*distance_away_from_edge*/);
            }
        }
        if(dir.y) //test horizontal walls
        {
            f32 t;
            if(will_hit_wall(dir.y, -ceily, dir.x, -wallx, wallx, &t))
            {
                pos = dir * (t + 0/*distance_away_from_edge*/); 
            }
            else if(will_hit_wall(dir.y, ceily, dir.x, -wallx, wallx, &t))
            {
                pos = dir * (t + 0/*distance_away_from_edge*/);
            }
        }
        result = pos;
        if (shape.type == SHAPE_RECT) rotate_t(result, shape.angle_t);
    } break;
    
    case SHAPE_CIRCLE: {
        result = dir*shape.dim.x; 
    } break;
    
    default: result = {}; assert (0);
    }
    return result + shape.pos;
}

#pragma pop_macro("debug_printf")