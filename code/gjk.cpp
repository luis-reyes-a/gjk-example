//NOTE this controls how many iterations of GJK we do to get values closer and closer to simplex
//not all these iterations will occur, though. We exit loop when we think we are done
#define GJK_MAX_ITERATION_COUNT 8
//NOTE this basically controls a how close a vector/dist must be for us to consider it zero 
#define GJK_ZERO_EPSILON 0.001f 

enum Shape_Type {
    SHAPE_CIRCLE,
    SHAPE_AABB,
};

struct Shape {
    Shape_Type type;
    Vector2 pos; //center of shape
    Vector2 dim; //for circle, x is radius
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
    
    #if 0
    case COLLIDER_RECT: {
        Vector2 vertices[4];
        Vector2 half_x_basis = eit2(shape->angle_t);
        Vector2 half_y_basis = perp(half_x_basis);
        half_x_basis *= 0.5f*shape->w;
        half_y_basis *= 0.5f*shape->h;
        
        vertices[0] = ( half_x_basis + half_y_basis);
        vertices[1] = (-half_x_basis + half_y_basis);
        vertices[2] = ( half_x_basis - half_y_basis);
        vertices[3] = (-half_x_basis - half_y_basis);
        
        result = find_closest_vertex(vertices, countof(vertices), dir);
    } break;
    #endif
    
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
    f32 smallest_dist_to_origin;
    
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
    
    void find_next_point() {
        simplex[2] = simplex[1];
        simplex[1] = simplex[0];
        simplex[0] = gjk_support(&s1, &s2, search_dir);
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
            f32 d1 = dot2(edge.dir, -simplex[index_to]);
            if (d1 <= 0) { 
                //NOTE if you imagine the edge line segment spanning along it's perp axis 
                //to make a rectangle infinitely grow on that axis, we're saying the origin lies on that rect
                //specifically on the side facing towards the origin.
                edge.origin_between_endpoints = true;
                edge.smallest_dist_to_origin  = dot2(edge.n_to_origin, -simplex[index_to]);
            } else { //otherwise closest point is at index_to
                edge.smallest_dist_to_origin = norm(simplex[index_to]);
            }
            
            //NOTE I don't expect this case to happen but just to be safe
            //if it's close enough, we say it falls on the edge line
            if (edge.smallest_dist_to_origin < GJK_ZERO_EPSILON) {
                edge.origin_lies_on_edge_line = true;
            }
            
        } else {
            //NOTE if we couldn't make a perpendicular direction, then that means the origin is along the edge line
            edge.origin_lies_on_edge_line = true;
            
            f32 edge_length = norm(edge.dir);
            assert (edge_length > GJK_ZERO_EPSILON);
            Vector2 edge_dir_n = edge.dir / edge_length;
            assert (!is_basically_zero(edge_dir_n));
            
            f32 dist = dot2(edge_dir_n, -simplex[index_from]);
            if (dist > edge_length) { //origin continues pass the edge line segment
                edge.smallest_dist_to_origin = dist - edge_length; 
            } else if (dist < 0) {
                //NOTE this case really should never happen, since we always pick new points going towards the origin
                edge.smallest_dist_to_origin = -dist; //to make it positive
            } else {
                edge.origin_between_endpoints = true;
                edge.smallest_dist_to_origin = 0;
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
    
    //NOTE here we make s2 and s1 pos local to s1 pos, in hopes of improving floating point precision 
    solver.s2.pos -= solver.s1.pos; 
    solver.s1.pos = {};
    
    solver.s1 = *s1;
    solver.s2 = *s2;
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

static GJK_Result
gjk_get_distance_apart(Shape s1, Shape s2) {
    GJK_Solver_2D solver = make_gjk_solver_2d(&s1, &s2);
    solver.find_next_point();
    
    GJK_Result result = {};  
    result.closest_dist = F32_MAX;
    
    if (is_basically_zero(solver.simplex[0])) {
        //we got lucky and instantly foind point where simplices overlap
        //but NOTE we have to do some work to find direction
        result.closest_dist = 0;
        goto END_OF_PROCEDURE;
    }
    
    //otherwise continue searching in opposite direction
    result.closest_dist = norm(solver.simplex[0]);
    solver.search_dir = -solver.simplex[0];
    solver.find_next_point();
    
    int simplex_contains_origin = 0; //we set to numbers instead of true to know where it was set
    if (is_basically_zero(solver.simplex[0] - solver.simplex[1])) {
        //degenerate line
        goto END_OF_PROCEDURE;
    } else if (is_basically_zero(solver.simplex[0])) {
        //we got lucky and instantly foind point where simplices overlap
        //but NOTE we have to do some work to find direction
        result.closest_dist = 0;
        return result;
    } else { //find new search dir
        GJK_Simplex_Edge edge10 = solver.make_edge(1, 0);
        if (edge10.origin_lies_on_edge_line) { //means we really can't keep searching
            if (edge10.origin_between_endpoints) { //means we should run EPA because there may be closer simplex point samples
                result = {};
                simplex_contains_origin = 1; //run EPA
            } else {
                //NOTE here we assume that closest_delta dir is beyond new point...
                result.closest_dist = edge10.smallest_dist_to_origin;
                result.closest_delta = -solver.simplex[0];
            }
            goto END_OF_PROCEDURE;
        } else { //we continue searching
            if (edge10.smallest_dist_to_origin < result.closest_dist) {
                result.closest_dist  = edge10.smallest_dist_to_origin;
                if (edge10.origin_between_endpoints) {
                    solver.search_dir = edge10.n_to_origin;
                    result.closest_delta = edge10.n_to_origin; //TODO mult with dist
                } else {
                    solver.search_dir = -solver.simplex[0];
                    result.closest_delta = -solver.simplex[0];
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
    
    //from this point on we have a "full simplex", which for 2D means a triangle
    for (int iteration_index = 0; iteration_index < GJK_MAX_ITERATION_COUNT; iteration_index += 1) {
        solver.find_next_point();
        if (is_basically_zero(solver.simplex[0])) {
            //got lucky 
            result.closest_dist  = 0;
            result.closest_delta = {};
            break;
        }
        
        Vector2 delta_10 = solver.simplex[0] - solver.simplex[1];
        if (is_basically_zero(delta_10)) {
            //degenerate line
            break;
        }
        
        Vector2 delta_21 = solver.simplex[1] - solver.simplex[2];
        assert (!is_basically_zero(delta_21)); //sanity check: we kept this line, so it should never be degenerate
        
        //NOTE here we test to see if we have a degenerate triangle case
        f32 collinearity = wedge2(delta_10, delta_21);
        if (absval(collinearity) < GJK_ZERO_EPSILON) { 
            //triangles are collinear
            break;
        }
        
        GJK_Simplex_Edge edge10 = solver.make_edge(1, 0);
        if (edge10.origin_lies_on_edge_line) {
            if (edge10.origin_between_endpoints) {
                result = {};
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
            } else if (edge20.smallest_dist_to_origin < result.closest_dist) {
                result.closest_dist  = edge10.smallest_dist_to_origin;
                result.closest_delta = -solver.simplex[0];
            }
            break;
        }
        
        
        Vector2 tri_center = (solver.simplex[0] + solver.simplex[1] + solver.simplex[2]) / 3.0f;
        Vector2 s0_to_tri_center = solver.simplex[0] - tri_center;
        f32 dot10 = dot2(edge10.n_to_origin, s0_to_tri_center); 
        f32 dot20 = dot2(edge10.n_to_origin, s0_to_tri_center);
        if (dot10 >= 0 && dot20 >= 0) {
            simplex_contains_origin = 2;
            break; //run epa
        }
        
        //otherwise we keep searching along the normal of the closest edge
        s32 index_to_replace = 0; //0 will never get replaced so we default to it
        if (edge10.smallest_dist_to_origin < edge20.smallest_dist_to_origin) {
            if (edge10.smallest_dist_to_origin >= result.closest_dist) {
                break; //we're getting farther away
            }
            
            result.closest_dist = edge10.smallest_dist_to_origin;
            if (edge10.origin_between_endpoints) {
                solver.search_dir = edge10.n_to_origin;
                result.closest_delta = edge10.n_to_origin;
            } else {
                solver.search_dir = -solver.simplex[0];
                result.closest_delta = -solver.simplex[0];
            }
        } else { //edge20 closest
            if (edge20.smallest_dist_to_origin >= result.closest_dist) {
                break; //we're getting farther away
            }
            
            result.closest_dist = edge10.smallest_dist_to_origin;
            if (edge10.origin_between_endpoints) {
                solver.search_dir = edge10.n_to_origin;
                result.closest_delta = edge10.n_to_origin;
            } else {
                solver.search_dir = -solver.simplex[0];
                result.closest_delta = -solver.simplex[0];
            }
        }
        
        assert (index_to_replace);
        solver.remove_point(index_to_replace);
    }
    
    END_OF_PROCEDURE:;
    if (simplex_contains_origin) {
        //do_epa();
    }
    return result;
         
}