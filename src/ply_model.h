#ifndef PLY_MODEL_H
#define PLY_MODEL_H

#include <string>

class PlyModel
{
public:
    PlyModel();

    void get_ply_model(const char* filename);

    int get_vertex_num();
    int get_face_num();

    float* get_model_vertices();
    unsigned int* get_model_faces();

    void print_all_lists();

private:
    int vertex_num;  // overall size of vertex list
    int face_num;  // overall size of face list

    int current_vertex;
    int current_face;

    float* vertex_list;
    unsigned int* face_list;

    void parse_vertex_line(std::string line);
    void parse_face_line(std::string line);
};

#endif
