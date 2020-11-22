#include "ply_model.h"

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
                int list_size = 3 * this->vertex_num;
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
        cout << "[" << i << "]: " << this->vertex_list[3 * i] << ", " << this->vertex_list[3 * i + 1] << ", " << this->vertex_list[3 * i + 2] << endl;
    }

    cout << "\nFace List: " << endl;
    for (int i = 0; i < this->face_num; ++i)
    {
        cout << "[" << i << "]: " << this->face_list[3 * i] << ", " << this->face_list[3 * i + 1] << ", " << this->face_list[3 * i + 2] << endl;
    }

    return;
}