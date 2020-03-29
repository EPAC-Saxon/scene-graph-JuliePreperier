#include <exception>
#include <stdexcept>
#include <chrono>
#if defined(_WIN32) || defined(_WIN64)
	#define WINDOWS_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include <iostream>
#endif
#include "Device.h"

namespace sgl {

	Device::Device(SDL_Window* sdl_window) 
	{
		// GL context.
		sdl_gl_context_ = SDL_GL_CreateContext(sdl_window); // /!\ sdl_gl_contexte existe déjà dans le .h
		SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
#if defined(__APPLE__)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// Initialize GLEW.
		if (GLEW_OK != glewInit())
		{
			throw std::runtime_error("couldn't initialize GLEW");
		}

		// Get the major and minor version of OpenGL
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major_version_);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor_version_);

		// Enable blending to 1 - source alpha.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Enable Z buffer.
		glEnable(GL_DEPTH_TEST);
	}

	void Device::Startup(std::pair<int, int> size)
	{
		// create the program using std::make_shared.
		program_ = std::make_shared<Program>();

		// Create the 2 shaders Vertex and Fragment.
		Shader vertexShader(ShaderType::VERTEX_SHADER);
		Shader fragmentShader(ShaderType::FRAGMENT_SHADER);

		if (!vertexShader.LoadFromFile("../Assets/SimpleVertex.glsl") ||
			!fragmentShader.LoadFromFile("../Assets/SimpleFragment.glsl")) {
			exit(-1);
		}

		// Add them to the program
		program_->AddShader(vertexShader);
		program_->AddShader(fragmentShader);

		//Link the program
		program_->LinkShader();

		//Use it
		program_->Use();

		//Create a matrix for the Perspective
		// Value taken from main.cpp -> SoftwareGL.sln (old project)
		// Theta = (90 * M_PI / 180), zfar = 1000, znear = 0.1f
		sgl::matrix projection(
			(size.second / size.first) * 1 / std::tan((90 * M_PI / 180) / 2), 0, 0, 0,
			0, 1 / std::tan((90 * M_PI / 180) / 2), 0, 0,
			0, 0, -(1000 + 0.1) / (1000 - 0.1), -1,
			0, 0, -(2 * 1000 * 0.1) / (1000 - 0.1), 0);

		// Set the uniform matrix in program for the projection
		program_->UniformMatrix("projection", projection);
		// Set the uniform matrix for the view
		program_->UniformMatrix("view ", camera_.LookAt());
		// Then an empty matrix for the model.
		program_->UniformMatrix("model", sgl::matrix());
		
	}


	void Device::Draw(const double dt)
	{
		// First you need to clean the screen and set the clear color using glClearColor and glClear.

		glClearColor(0.5, 0.5, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		//Then we need to go through all the scene.
		for (auto& node : scene_tree_) {
			// For each check if there is a mesh and if there is call the Draw method.
			if (node->GetLocalMesh()) {
				node->GetLocalMesh()->Draw(*program_, texture_manager_);
			}
		}
	}

} // End namespace sgl.
