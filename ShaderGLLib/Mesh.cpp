#include "Mesh.h"
#include <iterator>
#include <fstream>
#include <sstream>
#include <gl/glew.h>
#include "Mesh.h"

namespace sgl {
	struct Vertex {
		sgl::vector3 position_;
		sgl::vector3 normal_;
		sgl::vector2 texture_coord_;

	};

	Mesh::Mesh(const std::string& file)
	{
		auto maybe_obj = LoadFromObj(file);
		
		if (!maybe_obj) {
			throw std::runtime_error("Obj empty.");
		}


	}

	Mesh::~Mesh()
	{
#pragma message ("You have to complete this code!")
	}



	void Mesh::SetTextures(std::initializer_list<std::string> values)
	{
#pragma message ("You have to complete this code!")
	}

	void Mesh::Draw(
		const sgl::Program& program,
		const sgl::TextureManager& texture_manager,
		const sgl::matrix& model /*= {}*/) const
	{
#pragma message ("You have to complete this code!")
	}

	// TODO
	/*void Mesh::ComputeFlat(ObjFile obj)
	{
		std::vector<Vertex> vertices;
		//obj.indices[0] // PointIndex
		//obj.indices[1] // NormalIndex
		//obj.indices[2] // TextureIndex
		
		for (size_t i = 0; i < obj.indices[0].size(); ++i) {
			Vertex v;
			v.position_ = points[pointsIndex[i]];
			v.normal_ = normals[normalsIndex[i]];
			v.texture_coord_ = UVs[uvIndex[i]];

			flat_indices_.push_back(i);
			vertices.push_back(v);
			std::cout << "Position v N#: " << i << ": " << v.position_ << std::endl;
		}

		for (Vertex v : vertices) {
			flat_positions_.push_back(v.position_.x);
			flat_positions_.push_back(v.position_.y);
			flat_positions_.push_back(v.position_.z);

			flat_normals_.push_back(v.normal_.x);
			flat_normals_.push_back(v.normal_.y);
			flat_normals_.push_back(v.normal_.z);

			flat_textures_.push_back(v.texture_coord_.x);
			flat_textures_.push_back(v.texture_coord_.y);
		}

	}*/

	std::optional<sgl::Mesh::ObjFile> Mesh::LoadFromObj(const std::string& file)
	{
		ObjFile obj{};

		std::string line;
		std::ifstream ifs;
		// open file
		ifs.open(file);


		if (!ifs.is_open()) {
			throw std::runtime_error("Erreur à l'ouverture du fichier \n");
		}
		else
		{
			while (std::getline(ifs, line))
			{
				std::stringstream iss(line);
				char value;
				// iss enlève le caractère de la ligne. -> en faisant iss >> value on prend le premier caractère de la ligne et on l'enlève de la ligne.
				iss >> value;
				switch (value) {
				case 'v': {
					// iss peut lire tout seul la ligne et détecter une valeur dans celle-ci.
					value = iss.peek();
					if (value == 'n') {
						// si les première lettre sont vn ce ne sont pas des points mais des normals
						iss.ignore(256, ' ');
						sgl::vector3 vec;
						iss >> vec.x;
						iss >> vec.y;
						iss >> vec.z;

						obj.normals.push_back(vec);
					}
					else if (value == 't') {
						iss.ignore(256, ' ');
						sgl::vector2 vec;
						iss >> vec.x; // uv.u
						iss >> vec.y; // uv.v

						obj.textures.push_back(vec);
					}
					else {

						sgl::vector3 vec;
						iss >> vec.x;
						iss >> vec.y;
						iss >> vec.z;

						obj.positions.push_back(vec);
					}
					break;
				}
				case 'f':
					// f contient l'index du point(a), le UV (b) et l'index de la normal(c) -> a/b/c
					for (int i = 0; i < 3; i++) {
						std::string token;
						iss >> token;
						std::istringstream inneriss(token);
						std::string tok;

						std::getline(inneriss, tok, '/');
						float valIndicesPoint = atof(tok.c_str());

						std::getline(inneriss, tok, '/');
						float valIndicesTexture = atof(tok.c_str());

						std::getline(inneriss, tok, '/');
						float valIndicesNormal = atof(tok.c_str());

						obj.indices[0].fill(valIndicesPoint - 1); // PointIndex
						obj.indices[1].fill(valIndicesNormal - 1); // NormalIndex
						obj.indices[2].fill(valIndicesTexture - 1); // TextureIndex
					}
					break;
				}
			}
			ifs.close();
		}

		return obj;
	}

} // End namespace sgl.
