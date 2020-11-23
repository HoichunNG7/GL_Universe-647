#ifndef PLY_MODEL_H
#define PLY_MODEL_H

#include <string>

class PlyModel
{
public:
    PlyModel();

    void get_ply_model(const char* filename);

    void add_normal_vectors();

    int get_vertex_num();
    int get_face_num();

    float* get_model_vertices();
    unsigned int* get_model_faces();

    void print_all_lists();
    void print_bounding_box();

private:
    int vertex_num;  // overall size of vertex list
    int face_num;  // overall size of face list

    int current_vertex;
    int current_face;

    float* vertex_list;
    unsigned int* face_list;

    float max_coord[3]; // AABB bounding box (x_max, y_max, z_max)
    float min_coord[3]; // AABB bounding box (x_min, y_min, z_min)

    void parse_vertex_line(std::string line);
    void parse_face_line(std::string line);

    void get_3d_cross_product(float& r1, float& r2, float&r3, float x1, float x2, float x3, float y1, float y2, float y3);
};

#endif
