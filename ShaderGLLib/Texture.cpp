#include "Texture.h"
#include <assert.h>
#include <GL/glew.h>
#include "Image.h"

namespace sgl {

	Texture::Texture(const std::string& file)
	{
		Image img(file);
		size_ = img.GetSize();
		assert(img.size() % size_.first == 0);
		glGenTextures(1, &texture_id_);
		glBindTexture(GL_TEXTURE_2D, texture_id_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			static_cast<GLsizei>(size_.first),
			static_cast<GLsizei>(size_.second),
			0,
			GL_RGBA,
			GL_FLOAT,
			img.data());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	Texture::~Texture() 
	{
		glDeleteTextures(1, &texture_id_);
	}

	void Texture::Bind(const unsigned int slot /*= 0*/) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, texture_id_);
	}

	void Texture::UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	TextureManager::~TextureManager()
	{
		DisableAll();
	}

	bool TextureManager::AddTexture(
		const std::string& name, 
		const std::shared_ptr<sgl::Texture>& texture)
	{
		const auto [it, success] = name_texture_map_.insert(std::pair<std::string, std::shared_ptr<Texture>>(name.c_str(), texture));
		return success;
	}

	bool TextureManager::RemoveTexture(const std::string& name)
	{
		name_texture_map_.erase(name_texture_map_.find(name.c_str()));
		auto search = name_texture_map_.find(name.c_str());
		if (search != name_texture_map_.end()) {
			RemoveTexture(name.c_str());
			return false;

		}
		return true;
	}

	void TextureManager::EnableTexture(const std::string& name) const
	{
		auto search = name_texture_map_.find(name.c_str());
		if (search != name_texture_map_.end()) {	
			for (int i = 0; i < name_array_.size(); i++) {
				if (name_array_[i] != "") {
					name_array_[i] = name.c_str();
				}
			}
			throw std::runtime_error("No place found.");
		}
		else {
			throw std::runtime_error("Texture not found.");
		}
	}

	void TextureManager::DisableTexture(const std::string& name) const
	{
		auto search = name_texture_map_.find(name.c_str());
		if (search != name_texture_map_.end()) {
			for (int i = 0; i < name_array_.size(); i++) {
				if (name_array_[i] == name) {
					name_array_[i].empty();
				}
			}
			throw std::runtime_error("No place found.");
		}
		else {
			throw std::runtime_error("Texture not found.");
		}
	}

	void TextureManager::DisableAll() const
	{
		for (int i = 0; i < name_array_.size(); i++) {
			name_array_[i].empty();
		}
	}

} // End namespace sgl.
