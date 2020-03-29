#include "Mesh.h"
#include <iterator>
#include <fstream>
#include <sstream>
#include <gl/glew.h>
#include "Mesh.h"

namespace sgl {

	Mesh::Mesh(const std::string& file)
	{
		auto maybe_obj = LoadFromObj(file);
		
		if (!maybe_obj) {
			throw std::runtime_error("Obj empty.");
		}

		for (size_t i = 0; i < maybe_obj.value().indices.size(); ++i) {
			flat_indices_.push_back(i);

			flat_positions_.push_back(maybe_obj.value().positions[maybe_obj.value().indices[i][0]].x);
			flat_positions_.push_back(maybe_obj.value().positions[maybe_obj.value().indices[i][0]].y);
			flat_positions_.push_back(maybe_obj.value().positions[maybe_obj.value().indices[i][0]].z);

			flat_normals_.push_back(maybe_obj.value().normals[maybe_obj.value().indices[i][1]].x);
			flat_normals_.push_back(maybe_obj.value().normals[maybe_obj.value().indices[i][1]].y);
			flat_normals_.push_back(maybe_obj.value().normals[maybe_obj.value().indices[i][1]].z);

			flat_textures_.push_back(maybe_obj.value().textures[maybe_obj.value().indices[i][2]].x);
			flat_textures_.push_back(maybe_obj.value().textures[maybe_obj.value().indices[i][2]].y);

		}

		point_buffer_.BindCopy(flat_positions_.size() * sizeof(float), flat_positions_.data());
		normal_buffer_.BindCopy(flat_normals_.size() * sizeof(float), flat_normals_.data());
		texture_buffer_.BindCopy(flat_textures_.size() * sizeof(float), flat_textures_.data());
		index_buffer_.BindCopy(flat_indices_.size() * sizeof(float), flat_indices_.data());
		index_size_ = flat_indices_.size();

		glGenVertexArrays(1, &vertex_array_object_);
		glBindVertexArray(vertex_array_object_);
		
		/* Points */
		point_buffer_.Bind();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,NULL);
		point_buffer_.UnBind();
		
		/* Normals */
		normal_buffer_.Bind();
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		normal_buffer_.UnBind();

		/* Texture */
		texture_buffer_.Bind();
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		texture_buffer_.UnBind();

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	Mesh::~Mesh()
	{
		glDeleteVertexArrays(1, &vertex_array_object_);
	}



	void Mesh::SetTextures(std::initializer_list<std::string> values)
	{
		textures_.clear();
		textures_.assign(values);
		
	}

	void Mesh::Draw(
		const sgl::Program& program,
		const sgl::TextureManager& texture_manager,
		const sgl::matrix& model /*= {}*/) const
	{
		texture_manager.DisableAll();

		for (auto& tex : textures_) {
			texture_manager.EnableTexture(tex);
		}

		glBindVertexArray(vertex_array_object_);

		program.UniformMatrix("modelMat", model);

		index_buffer_.Bind();
		glDrawElements(GL_TRIANGLES, index_size_, GL_UNSIGNED_INT, nullptr);
		index_buffer_.UnBind();

		glBindVertexArray(0);
		texture_manager.DisableAll();
	}



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
