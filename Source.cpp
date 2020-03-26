#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Shader_v2.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>

class cars_logs_turtles
{
private:
	float length, b = 0.14f;
	std::vector<float> vertices;
	std::vector<short> ebo_indices;
	float pos_x, speed, inity = 0.0f;
	short direction, row_num;
	

	Shader sh;
	unsigned int vao, vbo, ebo;
public:
	cars_logs_turtles(float l, int level, char* vsp, char* fsp, short dir = -1)
	{		
		row_num = level;

		switch (level)
		{
		case 1:
			inity = -0.79f;
			speed = 0.65f;
			break;
		case 2:
			inity = -0.645f;
			speed = 0.35f;
			break;
		case 3:
			inity = -0.5f;
			speed = 0.50f;
			break;
		case 4:
			inity = -0.354f;
			speed = 0.85f;
			break;
		case 5:
			inity = -0.208f;
			speed = 0.63f;
			break;
		case 6:
			inity = 0.083f;
			speed = 0.34f;
			break;
		case 7:
			inity = 0.228f;
			speed = 0.53f;
			break;
		case 8:
			inity = 0.374f;
			speed = 0.74f;
			break;
		case 9:
			inity = 0.52f;
			speed = 0.65f;
			break;
		case 10:
			inity = 0.665f;
			speed = 0.45f;
			break;
		}

		length = l;	
		direction = dir;
		if (dir == 1)
			pos_x = -1.0f;
		else
			pos_x = 1.0f;

		ebo_indices = { 0,1,2,0,2,3 };
		
		//                 x            y         z   tex-x  tex-y
		vertices = { pos_x			, inity    , 0.0f, 0.0f, 0.0f,
					 pos_x - dir * l, inity    , 0.0f, 0.0f, 1.0f,
					 pos_x - dir * l, inity + b, 0.0f, 1.0f, 1.0f,
					 pos_x			, inity + b, 0.0f, 0.0f, 1.0f };

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), &vertices[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_indices.size() * sizeof(short), &ebo_indices[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);

		sh.init(vsp, fsp);
		sh.use();
	}
	void move_cars(float deltaT)
	{
		float disp = speed * deltaT;
		vertices[0] += direction * disp;
		vertices[5] += direction * disp;
		vertices[10] += direction * disp;
		vertices[15] += direction * disp;
		pos_x += direction * disp;

		// reset criteria
		if (pos_x * direction > (1 + length * 1.2))
		{
			vertices[0]  = -direction;
			vertices[5]  = -direction * (1 + length);
			vertices[10] = -direction * (1 + length);
			vertices[15] = -direction;

			pos_x = -direction;
		}
	}
	void draw_cars(float deltaT)
	{
		move_cars(deltaT);

		glm::mat4 model(1.0f);
		sh.setMat4("model", model);
		glBindVertexArray(vao); 
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), &vertices[0], GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	}
	float return_loc()
	{
		return pos_x;
	}
	~cars_logs_turtles()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}
	friend class frog;
};

class frog
{
private:
	float vert_loc, hor_loc;
	const float xoffs = 0.1f, yoffs = 0.1f;

	short num_completes = 0, move_dir = 0, y_level;
	std::vector<float> vertices;
	std::vector<short> ebo_indices;
	Shader sh;
	unsigned int vao, vbo, ebo;

	bool (frog::* lastfunc)(short, float);
public:
	frog(char* vsp, char* fsp)
	{
		hor_loc = -0.1f;
		vert_loc = -0.92f;
		y_level = 0;

		ebo_indices = { 0,1,2,0,2,3 };

		//					 x				   y         z   tex-x  tex-y
		vertices = { hor_loc		, vert_loc		  , 0.0f, 0.0f, 0.0f,
					 hor_loc + xoffs, vert_loc		  , 0.0f, 0.0f, 1.0f,
					 hor_loc + xoffs, vert_loc + yoffs, 0.0f, 1.0f, 1.0f,
					 hor_loc		, vert_loc + yoffs, 0.0f, 0.0f, 1.0f };

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), &vertices[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_indices.size() * sizeof(short), &ebo_indices[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);

		sh.init(vsp, fsp);
		sh.use();
	}
	bool processInput(GLFWwindow* window, float disp, float cond, frog& f, bool& no_move)
	{
		if (cond >= 0)
		{
			no_move = 0;
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(window, true);
				return false;
			}
			else if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
			{
				lastfunc = &frog::move_hor;
				move_dir = -1;
			}
			else if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
			{
				lastfunc = &frog::move_hor;
				move_dir = +1;
			}
			else if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
			{
				lastfunc = &frog::move_vert;
				move_dir = -1;
				--y_level;
			}
			else if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
			{
				lastfunc = &frog::move_vert;
				move_dir = +1;
				++y_level;
			}
			else
				return false;

			return (f.*lastfunc)(move_dir, disp);
		}
		else if (!no_move)
		{
			(f.*lastfunc)(move_dir, disp);
			return false;
		}
		else
			return false;
	}
	bool move_hor(short dir, float disp)
	{
		vertices[0] += dir * disp;
		vertices[5] += dir * disp;
		vertices[10] += dir * disp;
		vertices[15] += dir * disp;
		hor_loc += dir * disp;
		if (dir == 1)
		{
			vertices[3]  = 1.0f;
			vertices[4]  = 0.0f;
			vertices[8]  = 1.0f;
			vertices[9]  = 1.0f;
			vertices[13] = 0.0f;
			vertices[14] = 1.0f;
			vertices[18] = 0.0f;
			vertices[19] = 0.0f;
		}
		else
		{
			vertices[3]  = 0.0f;
			vertices[4]  = 1.0f;
			vertices[8]  = 0.0f;
			vertices[9]  = 0.0f;
			vertices[13] = 1.0f;
			vertices[14] = 0.0f;
			vertices[18] = 1.0f;
			vertices[19] = 1.0f;
		}
		return true;
	}
	bool move_vert(short dir, float disp)
	{
		if (y_level == 0 && dir == -1)
			return false;
		if (y_level == 12 && dir == 1)
		{
			completion();
			return true;
		}
		// Actually moving the frog
		vertices[1] += dir * disp;
		vertices[6] += dir * disp;
		vertices[11] += dir * disp;
		vertices[16] += dir * disp;
		vert_loc += dir * disp;
		if (dir == 1)
		{
			// Texture mapping changes
			vertices[3] = 0.0f;
			vertices[4] = 0.0f;
			vertices[8] = 1.0f;
			vertices[9] = 0.0f;
			vertices[13] = 1.0f;
			vertices[14] = 1.0f;
			vertices[18] = 0.0f;
			vertices[19] = 1.0f;
		}
		else
		{
			// Texture mapping changes
			vertices[3] = 1.0f;
			vertices[4] = 1.0f;
			vertices[8] = 0.0f;
			vertices[9] = 1.0f;
			vertices[13] = 0.0f;
			vertices[14] = 0.0f;
			vertices[18] = 1.0f;
			vertices[19] = 0.0f;
		}
		return true;
	}
	void draw()
	{		
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), &vertices[0], GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		glm::mat4 model(1.0f);
		sh.setMat4("model", model);
	}
	void collision(cars_logs_turtles v)
	{
		// check if bottom of from hits car
		if (vert_loc > v.inity && vert_loc < v.inity + v.b)
		{
			// check if left end hits car
			if (hor_loc > v.pos_x && hor_loc < v.pos_x + v.length)
				std::cout << "Collision\n";
			// check if right end hits car
			else if (hor_loc+xoffs > v.pos_x&& hor_loc + xoffs < v.pos_x + v.length)
				std::cout << "Collision\n";
		}
		else if (vert_loc + yoffs > v.inity&& vert_loc + yoffs < v.inity + v.b)
		{
			// check if left end hits car
			if (hor_loc > v.pos_x&& hor_loc < v.pos_x + v.length)
				std::cout << "Collision\n";
			// check if right end hits car
			else if (hor_loc + xoffs > v.pos_x&& hor_loc + xoffs < v.pos_x + v.length)
				std::cout << "Collision\n";
		}
	}
	void completion()
	{
		float top_row_div[] = {
			//-1.0f,   -0.9f, -0.7f,   -0.5f, -0.3f,   -0.1f, 0.1f,   0.3f, 0.5f,    0.7f, 0.9f,    1.0f  boundaries
			- 1.0f,   -0.9f, -0.7f,   -0.5f, -0.3f,   -0.1f, 0.1f,   0.3f, 0.5f,    0.7f, 0.9f,    1.0f
		};

		//if(hor_loc >= top_row_div[1] && hor_loc <= top_row_div[1] ||)
	}
	~frog()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}
};

void window_resize_fun(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Frogger", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Window creation failed\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, window_resize_fun);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	char vertexShaderpath[] = "D:\\C++\\OpenGL\\Tutorial\\Visual Studio\\Frogger\\frog.vs";
	char fragmentShaderpath[] = "D:\\C++\\OpenGL\\Tutorial\\Visual Studio\\Frogger\\frog.fs";

	frog f(vertexShaderpath, fragmentShaderpath);
	
	float vehicle_sizes[] = {  0.3f, 0.1f, 0.1f, 0.2f, 0.2f,
							   0.3f, 0.4f, 0.3f, 0.2f, 0.3f };

	std::vector<cars_logs_turtles*> vehicle_list;
	for (int i = 0; i < 10; ++i)
	{
		cars_logs_turtles* p = new cars_logs_turtles(vehicle_sizes[i], i+1, vertexShaderpath, fragmentShaderpath);
		vehicle_list.push_back(p);
	}

	float vel = 0.8f, disp;
	float currFrameTime, lastFrameTime = 0.0f, deltaTime, fps = 0;
	float time_till_next_input = 0.0f; 
	bool press = 0, no_move = 1;
	float cond;

	deltaTime = 0.001f;
	while (!glfwWindowShouldClose(window))
	{	
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		currFrameTime = lastFrameTime + deltaTime;
		//currFrameTime = glfwGetTime();
		//deltaTime = currFrameTime - lastFrameTime;
		lastFrameTime = currFrameTime;

		++fps;
		if (currFrameTime - int(currFrameTime) < 0.001)
		{
			std::cout << "frame render time [ms]: " << 1000 / fps << " : " << 1000 / 60 << std::endl;
			fps = 0;
		}
		

		disp = vel * deltaTime;
		
		cond = currFrameTime - time_till_next_input;
		if (cond >= 0)
		{
			cond -= 0.05; 
			no_move = 1;
		}
		press = f.processInput(window, disp, cond, f, no_move);
		if (press)
			time_till_next_input = currFrameTime + 0.14543/ vel;
		

		for (auto it : vehicle_list)
		{
			it->draw_cars(deltaTime);
			f.collision(*it);
		}
		f.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	for (auto it : vehicle_list)
		delete it;
	glfwTerminate();
	return 0;
}