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

class cars
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
	cars(float l, int level, char* vsp, char* fsp, int num_v, short dir = -1)
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
		default:
			std::cout << "Error, incorrect y-level\n";
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
	~cars()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}
	friend class frog;
};

class logs_turtles
{
private:
	float length, b = 0.14f;
	std::vector<float> vertices;
	std::vector<short> ebo_indices;
	float speed, inity = 0.0f;
	short direction, row_num, num_logturt;

	std::vector<float> posx, offsetx;
	short reset = 0;
	unsigned int vaoarr[3], vboarr[3];
	Shader sh;
	unsigned int ebo;
public:
	logs_turtles(float l, int level, char* vsp, char* fsp, int num_lgs, short dir = -1)
	{
		row_num = level;
		float pos_x, offset_x, inter_distance;
		switch (level)
		{
		case 7:
			inity = 0.083f;
			speed = 0.34f;
			inter_distance = 1.1f;
			break;
		case 8:
			inity = 0.228f;
			speed = 0.53f;
			inter_distance = 0.8f;
			break;
		case 9:
			inity = 0.374f;
			speed = 0.74f;
			inter_distance = 1.3f;
			break;
		case 10:
			inity = 0.52f;
			speed = 0.65f;
			inter_distance = 0.8f;
			break;
		case 11:
			inity = 0.665f;
			speed = 0.45f;
			inter_distance = 1.2f;
			break;
		}

		length = l;
		direction = dir;
		if (dir == 1)
			pos_x = -1.0f;
		else
			pos_x = 1.0f;

		ebo_indices = { 0,1,2,0,2,3 };
		offset_x = 0.0f;
		num_logturt = num_lgs;

		glGenVertexArrays(num_lgs, vaoarr);
		glGenBuffers(num_lgs, vboarr);
		glGenBuffers(1, &ebo);
		
		for (int i = 0; i < num_lgs; ++i)
		{
			offsetx.push_back(offset_x);
			
			float nxtlg = i * inter_distance;
			posx.push_back(pos_x - dir * nxtlg);
			//                 x						y     z   tex-x  tex-y
			vertices = { pos_x - dir * nxtlg      , inity    , 0.0f, 0.0f, 0.0f,
						 pos_x - dir * (l + nxtlg), inity    , 0.0f, 0.0f, 1.0f,
						 pos_x - dir * (l + nxtlg), inity + b, 0.0f, 1.0f, 1.0f,
						 pos_x - dir * nxtlg      , inity + b, 0.0f, 0.0f, 1.0f };
			
			glBindVertexArray(vaoarr[i]);
			glBindBuffer(GL_ARRAY_BUFFER, vboarr[i]);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), &vertices[0], GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_indices.size() * sizeof(short), &ebo_indices[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
			glEnableVertexAttribArray(1);
		}
		sh.init(vsp, fsp);
		sh.use();
	}
	void move_logs_turtles(float deltaT)
	{
		for (int i = 0; i < num_logturt; ++i)
		{
			offsetx[i] += direction * speed * deltaT;
			posx[i] += direction * speed * deltaT;

			// reset criteria
			if (posx[i] * direction > (1 + length * 1.05))
			{
				++reset;
				offsetx[i] = 0.0f;
				posx[i] = -direction;
			}
		}
	}
	void draw_logs_turtles(float deltaT)
	{
		move_logs_turtles(deltaT);

		
		for (int i = 0; i < num_logturt; ++i)
		{
			if (i >= reset)
				glBindVertexArray(vaoarr[i]);
			else
				glBindVertexArray(vaoarr[0]);
			//glBindVertexArray(vboarr[i]);
			glm::mat4 model(1.0f);
			glm::vec3 trans(offsetx[i], 0.0f, 0.0f);
			model = glm::translate(model, trans);
			sh.setMat4("model", model);			
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		}
	}
	~logs_turtles()
	{
		glDeleteVertexArrays(num_logturt, vaoarr);
		glDeleteBuffers(num_logturt, vboarr);
		glDeleteBuffers(1, &ebo);
	}
	friend class frog;
};

class finish
{
private:
	std::vector<short> fin_cols;
	unsigned int vao, vbo, ebo;
	Shader sh;
public:
	finish()
	{
		// empty constructor, does nothing
	}
	finish(char* vsp, char* fsp)
	{
		short ebo_indices[] = { 0,1,2,0,2,3 };
		fin_cols = { 0,0,0,0,0 };

		float top_row_div[] = {
	-0.90f,    -0.50f,    -0.10f,    0.30f,     0.70f
	//-1.0f,  -0.90f, -0.70f,   -0.50f, -0.30f,   -0.10f, 0.10f,   0.30f, 0.50f,    0.70f, 0.90f,    1.0f  boundaries
		};
		float hor_loc = top_row_div[0], xoffs = 0.2f, vert_loc = 0.8f, yoffs = 0.15f;
		float vertices[] = { hor_loc		  , vert_loc		, 0.0f, 0.0f, 0.0f,
							 hor_loc + xoffs  , vert_loc		, 0.0f, 0.0f, 1.0f,
							 hor_loc + xoffs  , vert_loc + yoffs, 0.0f, 1.0f, 1.0f,
							 hor_loc		  , vert_loc + yoffs, 0.0f, 0.0f, 1.0f };


		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ebo_indices), ebo_indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);

		sh.init(vsp, fsp);
		sh.use();
	}
	void set_fin_cols(short ind)
	{
		fin_cols[ind] = 1;
	}
	void draw()
	{
		float top_row_div[] = {
			-0.90f,    -0.50f,    -0.10f,    0.30f,     0.70f
			//-1.0f,  -0.90f, -0.70f,   -0.50f, -0.30f,   -0.10f, 0.10f,   0.30f, 0.50f,    0.70f, 0.90f,    1.0f  boundaries
		};
		
		float hor_loc, xoffs = 0.2f, vert_loc = 0.83f, yoffs = 0.15f;
		std::vector<float> vertices;
		for (int i = 0; i < 5; ++i)
		{
			if (fin_cols[i])
			{
				hor_loc = top_row_div[i];
				vertices = { hor_loc		  , vert_loc		, 0.0f, 0.0f, 0.0f,
									 hor_loc + xoffs  , vert_loc		, 0.0f, 0.0f, 1.0f,
									 hor_loc + xoffs  , vert_loc + yoffs, 0.0f, 1.0f, 1.0f,
									 hor_loc		  , vert_loc + yoffs, 0.0f, 0.0f, 1.0f };
				
				glm::mat4 model(1.0f);
				sh.setMat4("model", model);
				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
			}
		}
	}

};

class frog
{
private:
	float vert_offset, hor_offset, speed;
	const float xoffs = 0.1f, yoffs = 0.1f;
	const float vert_loc = -0.92f, hor_loc = -0.1f;
	std::vector<bool> finish_columns;

	short num_completes = 0, move_dir = 0, y_level;
	std::vector<float> vertices;
	std::vector<short> ebo_indices;
	Shader sh;
	unsigned int vao, vbo, ebo;

	float auto_vel = 0.0f;
	short (frog::* lastfunc)(short, float, finish&, float);
public:
	frog(char* vsp, char* fsp)
	{
		hor_offset  = 0.0f;
		vert_offset = 0.0f;
		y_level = 0;
		speed = 0.8f;

		ebo_indices = { 0,1,2,0,2,3 };
		finish_columns = { 0,0,0,0,0 };

		//				     x			         y         z   tex-x  tex-y
		vertices = { hor_loc		  , vert_loc		, 0.0f, 0.0f, 0.0f,
					 hor_loc + xoffs  , vert_loc		, 0.0f, 0.0f, 1.0f,
					 hor_loc + xoffs  , vert_loc + yoffs, 0.0f, 1.0f, 1.0f,
					 hor_loc		  , vert_loc + yoffs, 0.0f, 0.0f, 1.0f };

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
	short processInput(GLFWwindow* window, float deltaTime, float cond, frog *f, bool auto_move, finish& top_row)
	{
		if (cond >= 0)
		{
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(window, true);
				return 0;
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
				//std::cout << y_level << std::endl;
			}
			else if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
			{
				lastfunc = &frog::move_vert;
				move_dir = +1;
				++y_level;
				//std::cout << y_level << std::endl;
			}
			else
				return 0;

			return (*f.*lastfunc)(move_dir, deltaTime, top_row, 1.0f);
		}
		else if (auto_move)
		{
			(*f.*lastfunc)(move_dir, deltaTime, top_row, 1.0f);
			return 0;
		}
		else
			return 0;
	}
	short move_hor(short dir, float deltaT, finish& top_row, float change_speed = 1)
	{
		hor_offset += dir * deltaT * speed * change_speed;
		return 1;
	}
	short move_vert(short dir, float deltaT, finish& top_row, float unused)
	{
		if (y_level < 0 && dir == -1)
		{
			y_level = 0;
			return 0;
		}
		if (y_level > 11)
		{
			if (dir == 1)
			{
				completion(top_row);
				y_level = 11;
				return 3;
			}
		}
		// Actually moving the frog
		vert_offset += dir * deltaT * speed;
		return 2;
	}
	void draw()
	{		
		glBindVertexArray(vao);
		glm::mat4 model(1.0f);
		glm::vec3 trans(hor_offset, vert_offset, 0.0f);
		model = glm::translate(model, trans);
		sh.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	}
	void collision_cars(const cars& v)
	{
		// check collision in horizontal direction
		if (v.direction == -1)
		{
			if (hor_loc + hor_offset > v.pos_x&& hor_loc + hor_offset < v.pos_x + v.length)
				std::cout << "Collision\n";
			// check if right end hits car
			else if (hor_loc + hor_offset + xoffs > v.pos_x&& hor_loc + hor_offset + xoffs < v.pos_x + v.length)
				std::cout << "Collision\n";
		}
		else
		{
			if (hor_loc + hor_offset > v.pos_x - v.length && hor_loc + hor_offset < v.pos_x)
				std::cout << "Collision\n";
			// check if right end hits car
			else if (hor_loc + hor_offset + xoffs > v.pos_x - v.length && hor_loc + hor_offset + xoffs < v.pos_x)
				std::cout << "Collision\n";
		}
	}
	void collision_logs_turtles(logs_turtles& v, float deltaT)
	{
		bool collide = 0;
		// check collision in horizontal direction
		if (v.direction == -1)
		{
			for (int i = 0; i < v.num_logturt; ++i)
			{
				if (hor_loc + hor_offset > v.posx[i]&& hor_loc + hor_offset < v.posx[i] + v.length)
				{
					collide = 1; 
					break;
				}
				// check if right end hits car
				else if (hor_loc + hor_offset + xoffs > v.posx[i]&& hor_loc + hor_offset + xoffs < v.posx[i] + v.length)
				{
					collide = 1; 
					break;
				}
			}
		}
		else
		{
			for (int i = 0; i < v.num_logturt; ++i)
			{
				if (hor_loc + hor_offset > v.posx[i] - v.length && hor_loc + hor_offset < v.posx[i])
				{
					collide = 1; break;
				}
				// check if right end hits car
				else if (hor_loc + hor_offset + xoffs > v.posx[i] - v.length && hor_loc + hor_offset + xoffs < v.posx[i])
				{
					collide = 1;
					break;
				}
			}
		}
		if (collide)
		{
			finish unused;
			move_hor(v.direction, deltaT, unused, v.speed / speed);
			if (hor_loc + hor_offset > 1.0f || hor_loc + hor_offset < -1.0f)
				std::cout << "Drowned\n";
			else
				draw();
		}
		else
		{
			//std::cout << "Drowned\n";
			draw();
		}
	}
	float return_speed()
	{
		return speed;
	}
	short return_level()
	{
		return y_level;
	}
	void completion(finish& top_row)
	{
		float top_row_div[] = {
			//-1.0f,  -0.90f, -0.70f,   -0.50f, -0.30f,   -0.10f, 0.10f,   0.30f, 0.50f,    0.70f, 0.90f,    1.0f  boundaries
			          -0.93f, -0.63f,   -0.53f, -0.23f,   -0.13f, 0.03f,   0.27f, 0.43f,    0.67f, 0.83f,   
		};


		bool c1, c2, c3, c4, c5;
		short fin_col;
		c1 = hor_loc + hor_offset >= top_row_div[0] && hor_loc + hor_offset <= top_row_div[1]; if (c1 == 1) fin_col = 0;
		c2 = hor_loc + hor_offset >= top_row_div[2] && hor_loc + hor_offset <= top_row_div[3]; if (c2 == 1) fin_col = 1;
		c3 = hor_loc + hor_offset >= top_row_div[4] && hor_loc + hor_offset <= top_row_div[5]; if (c3 == 1) fin_col = 2;
		c4 = hor_loc + hor_offset >= top_row_div[6] && hor_loc + hor_offset <= top_row_div[7]; if (c4 == 1) fin_col = 3;
		c5 = hor_loc + hor_offset >= top_row_div[8] && hor_loc + hor_offset <= top_row_div[9]; if (c5 == 1) fin_col = 4;
		
		if ((c1 || c2 || c3 || c4 || c5) && finish_columns[fin_col]==false)
		{
			finish_columns[fin_col] = true;
			top_row.set_fin_cols(fin_col);
			
		}
		else
			std::cout << "Death. \n";
		
	}
	void reset_frog()
	{
		hor_offset = 0.0f;
		vert_offset = 0.0f;
		y_level = 0;
		draw();
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

	frog* f = new frog(vertexShaderpath, fragmentShaderpath);
	
	float vehicle_sizes[] = { 0.3f, 0.1f, 0.1f, 0.2f, 0.2f };
	float logturt_sizes[] = { 0.3f, 0.4f, 0.3f, 0.2f, 0.3f };
	float vehicle_dir[] = { 1, -1, -1, 1, 1 };
	float logturt_dir[] = { 1, -1, -1, 1, 1 };
	float vehicle_num[] = { 0, 0, 0, 0, 1 };
	float logturt_num[] = { 2, 3, 2, 3, 2 };

	std::vector<cars*> vehicle_list;
	std::vector<logs_turtles*> logs_list;
	for (int i = 0; i < 5; ++i)
	{
		cars* p = new cars(vehicle_sizes[i], i + 1, vertexShaderpath, fragmentShaderpath, vehicle_num[i], vehicle_dir[i]);
		vehicle_list.push_back(p);
	}
	for (int i = 0; i < 5; ++i)
	{
		logs_turtles* p = new logs_turtles(logturt_sizes[i], i + 7, vertexShaderpath, fragmentShaderpath, logturt_num[i], logturt_dir[i]);
		logs_list.push_back(p);
	}
	
	finish fin(vertexShaderpath, fragmentShaderpath);


	float vel = f->return_speed();
	float currFrameTime, lastFrameTime = 0.0f, deltaTime, fps = 0;
	float time_till_next_input = 0.0f; 
	short press = 0, frog_level; bool auto_move = 1;
	float cond;

	bool lvl_comp_crit = 0;
//	deltaTime = 0.001f;
	while (!glfwWindowShouldClose(window))
	{	
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		//currFrameTime = lastFrameTime + deltaTime;
		currFrameTime = glfwGetTime();
		deltaTime = currFrameTime - lastFrameTime;
		lastFrameTime = currFrameTime;

		++fps;
		if (currFrameTime - int(currFrameTime) < 0.001)
		{
			std::cout << "frame render time [ms]: " << 1000 / fps << " : " << 1000 / 60 << std::endl;
			fps = 0;
		}
		
		if (press != 3)
		{
			cond = currFrameTime - time_till_next_input;
			if (cond >= 0)
			{
				// to provide a buffer to prevent seamless movement of player
				cond -= 0.05;
				auto_move = 0;
			}
			press = f->processInput(window, deltaTime, cond, f, auto_move, fin);
			if (!press) {}
			else if (press == 3){ }
			else
			{
				time_till_next_input = currFrameTime + 0.14543 / vel;
				auto_move = 1;
			}

			frog_level = f->return_level();

			if (!auto_move)
			{
				if (frog_level > 0 && frog_level < 6)
					f->collision_cars(*vehicle_list[frog_level - 1]);
			}
			if (frog_level > 6 && frog_level < 12 && !auto_move)
				f->collision_logs_turtles(*logs_list[frog_level - 7], deltaTime);
			else
				f->draw();
		}
		else
		{
			// level completion criteria met, animate frog going into brush
			// reset the cars
			for (int i=0;i<5;++i)
			{
				delete vehicle_list[i];
				cars* p = new cars(vehicle_sizes[i], i + 1, vertexShaderpath, fragmentShaderpath, vehicle_num[i], vehicle_dir[i]);
				vehicle_list[i] = p;
			}
			//reset the logs
			for (int i = 0; i < 5; ++i)
			{
				delete logs_list[i];
				logs_turtles* p = new logs_turtles(logturt_sizes[i], i + 7, vertexShaderpath, fragmentShaderpath, logturt_num[i], logturt_dir[i]);
				logs_list[i] = p;
			}
			press = 0;
			// reset the frog
			f->reset_frog();
		}
		// draw cars3
		for (auto it : vehicle_list)
			it->draw_cars(deltaTime);

		// draw logs and turtles
		for (auto it : logs_list)
			it->draw_logs_turtles(deltaTime);

		// draw completed columns
		fin.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	for (auto it : vehicle_list)
		delete it;
	glfwTerminate();
	return 0;
}