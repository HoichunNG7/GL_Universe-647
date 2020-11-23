#include "ply_model.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void PlyModel::get_ply_model(const char* filename)
{
    ifstream plyObject;
    char plyNewLine[128];
    string newLineStr;
    int readSectionFlag = 0; // 0-header, 1-vertex, 2-face

    plyObject.open(filename);
    if (plyObject.fail())
    {
        cout << "Fail to open file: " << filename << endl;
        return;
    }

    do
    {
        if (readSectionFlag == 0) // read header
        {
            plyObject.getline(plyNewLine, 128);
            newLineStr = plyNewLine;
            // cout << plyNewLine << endl; // test

            if ("element vertex" == newLineStr.substr(0, 14))
            {
                this->vertex_num = stoi(newLineStr.substr(15));
                int list_size = 6 * this->vertex_num;
                this->vertex_list = new float[list_size];
            }
            else if ("element face" == newLineStr.substr(0, 12))
            {
                this->face_num = stoi(newLineStr.substr(13));
                int list_size = 3 * this->face_num;
                this->face_list = new unsigned int[list_size];
            }
            else if ("end_header" == newLineStr)
            {
                readSectionFlag = 1;
            }
        }
        else if (readSectionFlag == 1) // read vertex list
        {
            for (int i = 0; i < this->vertex_num; ++i)
            {
                plyObject.getline(plyNewLine, 128);
                newLineStr = plyNewLine;
                // cout << plyNewLine << endl; // test

                this->parse_vertex_line(newLineStr);
            }

            readSectionFlag = 2;
        }
        else if (readSectionFlag == 2) // read face list
        {
            for (int i = 0; i < this->face_num; ++i)
            {
                plyObject.getline(plyNewLine, 128);
                newLineStr = plyNewLine;
                // cout << plyNewLine << endl; // test

                this->parse_face_line(newLineStr);
            }

            break;
        }
    } while (!plyObject.eof());

    plyObject.close();
    return;
}

PlyModel::PlyModel()
{
    this->vertex_num = 0; // overall size of vertex list
    this->face_num = 0; // overall size of face list
    this->current_vertex = -1;
    this->current_face = -1;
    this->vertex_list = NULL;
    this->face_list = NULL;
    return;
}

int PlyModel::get_vertex_num()
{
    return this->vertex_num;
}

int PlyModel::get_face_num()
{
    return this->face_num;
}

float* PlyModel::get_model_vertices()
{
    return this->vertex_list;
}

unsigned int* PlyModel::get_model_faces()
{
    return this->face_list;
}

void PlyModel::parse_vertex_line(string line)
{
    string temp;

    istringstream is(line);
    for (int i = 0; i < 3; ++i)
    {
        is >> temp;
        this->current_vertex++;
        this->vertex_list[this->current_vertex] = stof(temp);
    }

    this->current_vertex += 3;
    return;
}

void PlyModel::parse_face_line(string line)
{
    string temp;

    istringstream is(line);
    is >> temp;
    for (int i = 0; i < 3; ++i)
    {
        is >> temp;
        this->current_face++;
        this->face_list[this->current_face] = (unsigned int)stoi(temp);
    }

    return;
}

void PlyModel::print_all_lists()
{
    cout << "Vertex List: " << endl;
    for (int i = 0; i < this->vertex_num; ++i)
    {
        cout << "[" << i << "]: " << this->vertex_list[6 * i] << ", " << this->vertex_list[6 * i + 1] << ", " << this->vertex_list[6 * i + 2] << endl;
        cout << this->vertex_list[6 * i + 3] << ", " << this->vertex_list[6 * i + 4] << ", " << this->vertex_list[6 * i + 5] << endl;
    }

    cout << "\nFace List: " << endl;
    for (int i = 0; i < this->face_num; ++i)
    {
        cout << "[" << i << "]: " << this->face_list[3 * i] << ", " << this->face_list[3 * i + 1] << ", " << this->face_list[3 * i + 2] << endl;
    }

    return;
}

void PlyModel::add_normal_vectors()
{
    if (this->vertex_num == 0 || this->face_num == 0)
        return;

    float r1, r2, r3;
    int v1, v2, v3; // vertex indices from a face
    float vec_length; // length of a vector
    int v_num = this->vertex_num;
    float** normal_vecs = new float* [v_num];
    for (int i = 0; i < v_num; ++i)
    {
        normal_vecs[i] = new float[3];
        normal_vecs[i][0] = 0;
        normal_vecs[i][1] = 0;
        normal_vecs[i][2] = 0;
    }

    for (int i = 0; i < this->face_num; ++i)
    {
        v1 = this->face_list[3 * i];
        v2 = this->face_list[3 * i + 1];
        v3 = this->face_list[3 * i + 2];

        this->get_3d_cross_product(r1, r2, r3, this->vertex_list[6 * v2] - this->vertex_list[6 * v1], this->vertex_list[6 * v2 + 1] - this->vertex_list[6 * v1 + 1], this->vertex_list[6 * v2 + 2] - this->vertex_list[6 * v1 + 2]
            , this->vertex_list[6 * v3] - this->vertex_list[6 * v1], this->vertex_list[6 * v3 + 1] - this->vertex_list[6 * v1 + 1], this->vertex_list[6 * v3 + 2] - this->vertex_list[6 * v1 + 2]);
        vec_length = sqrt(r1 * r1 + r2 * r2 + r3 * r3);

        normal_vecs[v1][0] += r1 / vec_length;
        normal_vecs[v2][0] += r1 / vec_length;
        normal_vecs[v3][0] += r1 / vec_length;
        normal_vecs[v1][1] += r2 / vec_length;
        normal_vecs[v2][1] += r2 / vec_length;
        normal_vecs[v3][1] += r2 / vec_length;
        normal_vecs[v1][2] += r3 / vec_length;
        normal_vecs[v2][2] += r3 / vec_length;
        normal_vecs[v3][2] += r3 / vec_length;
    }

    for (int i = 0; i < this->vertex_num; ++i)
    {
        vec_length = sqrt(normal_vecs[i][0] * normal_vecs[i][0] + normal_vecs[i][1] * normal_vecs[i][1] + normal_vecs[i][2] * normal_vecs[i][2]);
        this->vertex_list[6 * i + 3] = normal_vecs[i][0] / vec_length;
        this->vertex_list[6 * i + 4] = normal_vecs[i][1] / vec_length;
        this->vertex_list[6 * i + 5] = normal_vecs[i][2] / vec_length;
    }

    delete []normal_vecs;
    return;
}

void PlyModel::get_3d_cross_product(float& r1, float& r2, float& r3, float x1, float x2, float x3, float y1, float y2, float y3)
{
    r1 = x2 * y3 - x3 * y2;
    r2 = x3 * y1 - x1 * y3;
    r3 = x1 * y2 - x2 * y1;
    return;
}
