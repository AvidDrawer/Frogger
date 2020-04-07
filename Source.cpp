/* Frogger.
Completed initial version- moving cars, moving logs, death criteria, end criteria

Potential Improvements-
1.  Add multiple cars/row, similar to the logs implementation
2.  Add animations
		- for when the frog falls off
		- when becoming roadkill (just image)
		- top row criteria
	    - when the frog moves (add a different image for the auto-move duration?)
3.  Add turtles, crocodiles
4.  Add female frog and snake on the logs
5.  Implement stable level-up changes
	 - only one level present now and it loops over
6.  Implement number of lives (increase and decrease)
7.  Display score, #lives
8.  Use a more consistent version of the frog images when it is oriented in different directions
9.  Perhaps, use a single drawing class and make everyone else friends. Then a single draw call can be implemented for everybody instead
    a separate draw in each class.
10. Game is based on internal clock timing. Account for what happens in-case of system overload and the game freezes for a moment. Bascially, sync-desync clock
11. Use a divider-less road texture for the highest road level
12. Resolution-based check on textures for better viewing experience, size check on the .png files
*/

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
#include <typeinfo>

class cars	
{
private:
	float length, b = 0.12f;
	std::vector<float> vertices;
	std::vector<short> ebo_indices;
	float pos_x, speed, inity = 0.0f;
	short direction, row_num;
	

	Shader sh;
	unsigned int vao, vbo, ebo, tex;
public:
	cars(float l, int level, char* vsp, char* fsp, std::string texpath, short num_v, short dir = -1)
	{		
		row_num = level;

		switch (level)
		{
		case 1:
			inity = -0.79f;
			speed = 0.65f;
			break;
		case 2:
			inity = -0.65f;
			speed = 0.35f;
			break;
		case 3:
			inity = -0.51f;
			speed = 0.50f;
			break;
		case 4:
			inity = -0.37f;
			speed = 0.85f;
			break;
		case 5:
			inity = -0.23f;
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
					 pos_x - dir * l, inity    , 0.0f, 1.0f, 0.0f,
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

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, numchan;
		unsigned char* data = stbi_load(texpath.c_str(), &width, &height, &numchan, 4);
		if (data == NULL)
			std::cout << "Texture loading  failed\n";
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);
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
		sh.setInt("texture1", 0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(vao); 
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), &vertices[0], GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		//glBindTexture(GL_TEXTURE_2D, 0);
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
	float length, b = 0.12f;
	std::vector<float> vertices;
	std::vector<short> ebo_indices;
	float speed, inity = 0.0f;
	short direction, row_num, num_logturt;

	std::vector<float> posx, offsetx;
	short reset = 0;
	unsigned int vaoarr[3], vboarr[3];
	Shader sh;
	unsigned int ebo, tex;
public:
	logs_turtles(float l, int level, char* vsp, char* fsp, std::string texpath, short num_lgs, short dir = -1)
	{
		row_num = level;
		float pos_x, offset_x, inter_distance;
		switch (level)
		{
		case 7:
			inity = 0.11f;
			speed = 0.34f;
			inter_distance = 1.1f;
			break;
		case 8:
			inity = 0.25f;
			speed = 0.53f;
			inter_distance = 0.8f;
			break;
		case 9:
			inity = 0.39f;
			speed = 0.74f;
			inter_distance = 1.3f;
			break;
		case 10:
			inity = 0.53f;
			speed = 0.65f;
			inter_distance = 0.8f;
			break;
		case 11:
			inity = 0.67f;
			speed = 0.45f;
			inter_distance = 1.2f;
			break;
		default:
			std::cout << "Error, incorrect level use in log_turtle constructor <switch case>\n";
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
						 pos_x - dir * (l + nxtlg), inity    , 0.0f, 1.0f, 0.0f,
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

			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			int width, height, numchan;
			unsigned char* data = stbi_load(texpath.c_str(), &width, &height, &numchan, 4);
			if (data == NULL)
				std::cout << "Texture loading error\n";
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			stbi_image_free(data);
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

		glBindTexture(GL_TEXTURE_2D, tex);
		glActiveTexture(GL_TEXTURE0);
		sh.setInt("texture1", 0);
		for (int i = 0; i < num_logturt; ++i)
		{
			if (i >= reset)
				glBindVertexArray(vaoarr[i]);
			else
				glBindVertexArray(vaoarr[0]);
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
		glDeleteTextures(1, &tex);
	}
	friend class frog;
};

class finish
{
private:
	std::vector<short> fin_cols;
	short num_fin;
	unsigned int vao, vbo, ebo, tex[2];
	Shader sh;
public:
	finish()
	{
		// empty constructor, does nothing
	}
	finish(char* vsp, char* fsp, std::vector<std::string> texpath)
	{
		short ebo_indices[] = { 0,1,2,0,2,3 };
		fin_cols = { 0,0,0,0,0 };
		num_fin = 0;

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

		glGenTextures(2, tex);
		// no frog state
		glBindTexture(GL_TEXTURE_2D, tex[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, numchan;
		unsigned char* data = stbi_load(texpath[0].c_str(), &width, &height, &numchan, 4);
		if (data == NULL)
			std::cout << "Texture loading error\n";
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		// resident-frog state
		glBindTexture(GL_TEXTURE_2D, tex[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		data = stbi_load(texpath[1].c_str(), &width, &height, &numchan, 4);
		if (data == NULL)
			std::cout << "Texture loading error\n";
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		sh.init(vsp, fsp);
		sh.use();
	}
	void set_fin_cols(short ind)
	{
		fin_cols[ind] = 1;
		num_fin++;
	}
	short get_numfins()
	{
		return num_fin;
	}
	void draw()
	{
		float top_row_div[] = {
			-0.90f,    -0.50f,    -0.10f,    0.30f,     0.70f
			//-1.0f,  -0.90f, -0.70f,   -0.50f, -0.30f,   -0.10f, 0.10f,   0.30f, 0.50f,    0.70f, 0.90f,    1.0f  boundaries
		};
		
		float hor_loc, xoffs = 0.2f, vert_loc = 0.80f, yoffs = 0.15f;
		std::vector<float> vertices;
		for (int i = 0; i < 5; ++i)
		{			
			hor_loc = top_row_div[i];
			vertices = { hor_loc		  , vert_loc		, 0.0f, 0.0f, 0.0f,
						 hor_loc + xoffs  , vert_loc		, 0.0f, 1.0f, 0.0f,
						 hor_loc + xoffs  , vert_loc + yoffs, 0.0f, 1.0f, 1.0f,
						 hor_loc		  , vert_loc + yoffs, 0.0f, 0.0f, 1.0f };
			
			glm::mat4 model(1.0f);
			sh.setMat4("model", model);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
			if (fin_cols[i])
			{
				glBindTexture(GL_TEXTURE_2D, tex[1]);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, tex[0]);
			}
			glActiveTexture(GL_TEXTURE0);
			sh.setInt("texture1", 0);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		}
	}

};

class frog
{
private:
	float vert_offset, hor_offset, speed;
	const float xoffs = 0.08f, yoffs = 0.08f;
	const float vert_loc = -0.92f, hor_loc = -0.1f;
	std::vector<bool> finish_columns;

	short num_completes = 0, move_dir = 0, y_level, y_prev_level, move_state;
	std::vector<float> vertices;
	std::vector<short> ebo_indices;
	Shader sh;
	unsigned int vao, vbo, ebo, tex[4];

	float auto_vel = 0.0f;
	short (frog::* lastfunc)(short, float, finish*, float);
public:
	frog(char* vsp, char* fsp, std::vector<std::string> texpath)
	{
		hor_offset  = 0.0f;
		vert_offset = 0.0f;
		y_level = 0;
		y_prev_level = 0;
		speed = 0.8f;
		move_state = 1;

		ebo_indices = { 0,1,2,0,2,3 };
		finish_columns = { 0,0,0,0,0 };

		//				     x			         y         z   tex-x  tex-y
		vertices = { hor_loc		  , vert_loc		, 0.0f, 0.0f, 0.0f,
					 hor_loc + xoffs  , vert_loc		, 0.0f, 1.0f, 0.0f,
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

		glGenTextures(4, tex);
		// up state
		glBindTexture(GL_TEXTURE_2D, tex[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, numchan;
		unsigned char* data = stbi_load(texpath[0].c_str(), &width, &height, &numchan, 4);
		if (data == NULL)
			std::cout << "Texture loading error\n";
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		// down state
		glBindTexture(GL_TEXTURE_2D, tex[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		data = stbi_load(texpath[1].c_str(), &width, &height, &numchan, 4);
		if (data == NULL)
			std::cout << "Texture loading error\n";
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		// left state
		glBindTexture(GL_TEXTURE_2D, tex[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		data = stbi_load(texpath[2].c_str(), &width, &height, &numchan, 4);
		if (data == NULL)
			std::cout << "Texture loading error\n";
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		// right state
		glBindTexture(GL_TEXTURE_2D, tex[3]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		data = stbi_load(texpath[3].c_str(), &width, &height, &numchan, 4);
		if (data == NULL)
			std::cout << "Texture loading error\n";
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		sh.init(vsp, fsp);
		sh.use();
	}
	short processInput(GLFWwindow* window, float deltaTime, float cond, frog *f, bool auto_move, finish *top_row)
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
				move_state = 3;
			}
			else if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
			{
				lastfunc = &frog::move_hor;
				move_dir = +1;
				move_state = 4;
			}
			else if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
			{
				lastfunc = &frog::move_vert;
				move_dir = -1;
				move_state = 2;
				y_prev_level = y_level--;
				//std::cout << y_level << std::endl;
			}
			else if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
			{
				lastfunc = &frog::move_vert;
				move_dir = +1;
				move_state = 1;
				y_prev_level = y_level++;
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
	short move_hor(short dir, float deltaT, finish* top_row, float change_speed = 1)
	{
		// The horizontal movement can be due to frog movement, or due to log movement or both.
		float deltaS = dir * deltaT * speed * change_speed;
		if(change_speed != 1)   // means this is due to log movement, so no restriction on movement
			hor_offset += deltaS;
		else
		{
			float autodist = dir * 0.14f / 2; //auto-move distance
			if (hor_loc + hor_offset + yoffs + deltaS + autodist >= 1.0f) {}
			else if (hor_loc + hor_offset + deltaS + autodist <= -1.0f) {}
			else
				hor_offset += deltaS;
		}
		return 1;
	}
	short move_vert(short dir, float deltaT, finish* top_row, float unused)
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
		
		glActiveTexture(GL_TEXTURE0);
		sh.setInt("texture1", 0);
		switch (move_state)
		{
		case 1:
			glBindTexture(GL_TEXTURE_2D, tex[0]); break;
		case 2:
			glBindTexture(GL_TEXTURE_2D, tex[1]); break;
		case 3:
			glBindTexture(GL_TEXTURE_2D, tex[2]); break;
		case 4:
			glBindTexture(GL_TEXTURE_2D, tex[3]); break;
		}
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	}
	short collision_cars(const cars& v)
	{
		// check collision in horizontal direction
		if (v.direction == -1)
		{
			if (hor_loc + hor_offset > v.pos_x&& hor_loc + hor_offset < v.pos_x + v.length)
			{
				return 3; std::cout << "Collision\n";
			}
			// check if right end hits car
			else if (hor_loc + hor_offset + xoffs > v.pos_x&& hor_loc + hor_offset + xoffs < v.pos_x + v.length)
			{
				return 3; std::cout << "Collision\n";
			}
		}
		else
		{
			if (hor_loc + hor_offset > v.pos_x - v.length && hor_loc + hor_offset < v.pos_x)
			{
				return 3; std::cout << "Collision\n";
			}
			// check if right end hits car
			else if (hor_loc + hor_offset + xoffs > v.pos_x - v.length && hor_loc + hor_offset + xoffs < v.pos_x)
			{
				return 3; std::cout << "Collision\n";
			}

		}
		return 1; //
		/*std::cout << (typeid(v)==typeid(y_level));
		std::cout << "A";*/
	}
	short collision_logs_turtles(logs_turtles& v, float deltaT)
	{
		bool collide = 0;
		// check collision in horizontal direction
		// on logs and turtles, collision means that frog is on a floating object. Otherwise, it drowns!!!
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
			finish* unused = new finish();
			move_hor(v.direction, deltaT, unused, v.speed / speed);
			delete unused;
			if (hor_loc + hor_offset > 1.0f || hor_loc + hor_offset + xoffs < -1.0f)
			{
				return 3;
				//std::cout << "Drowned\n";
			}
			else
				draw();
			return 1;
		}
		else
		{
			//std::cout << "Drowned\n";
			return 3;
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
	short return_prev_level()
	{
		return y_prev_level;
	}
	void completion(finish* top_row)
	{
		float top_row_div[] = {
			//-1.0f,  -0.90f, -0.70f,   -0.50f, -0.30f,   -0.10f, 0.10f,   0.30f, 0.50f,    0.70f, 0.90f,    1.0f  boundaries
					  -0.92f, -0.77f,   -0.52f, -0.37f,   -0.12f, 0.03f,   0.27f, 0.42f,    0.67f, 0.82f,
		};


		bool c1, c2, c3, c4, c5;
		short fin_col;
		c1 = hor_loc + hor_offset >= top_row_div[0] && hor_loc + hor_offset <= top_row_div[1]; if (c1 == 1) fin_col = 0;
		c2 = hor_loc + hor_offset >= top_row_div[2] && hor_loc + hor_offset <= top_row_div[3]; if (c2 == 1) fin_col = 1;
		c3 = hor_loc + hor_offset >= top_row_div[4] && hor_loc + hor_offset <= top_row_div[5]; if (c3 == 1) fin_col = 2;
		c4 = hor_loc + hor_offset >= top_row_div[6] && hor_loc + hor_offset <= top_row_div[7]; if (c4 == 1) fin_col = 3;
		c5 = hor_loc + hor_offset >= top_row_div[8] && hor_loc + hor_offset <= top_row_div[9]; if (c5 == 1) fin_col = 4;

		if ((c1 || c2 || c3 || c4 || c5) && finish_columns[fin_col] == false)
		{
			finish_columns[fin_col] = true;
			top_row->set_fin_cols(fin_col);

		}
		else
		{
			//std::cout << "Death. \n";
		}
		
	
		
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

class bg_draw
{
private: 
	std::vector<short> indices;
	std::vector<float> rect;
	bool texture_use;
	unsigned int vao, vbo, ebo, tex;
	Shader sh;
public:
	bg_draw(int level, char* vsp, char* fsp, char * texpath, bool texuse = 1)
	{
		texture_use = texuse;

		float inity, b = 0.14f;
		if (level == 0)
			inity = -1.0f;
		else
			inity = -0.8f + 0.14f * (level - 1);
		
		if (level == 0 || level == 6 || level == 12)
			b = 0.2f;
		if (level > 6)
			inity += 0.06f;

		rect = { -1.0f, inity    , 0.0f, 0.0f, 0.0f,   // bot left
				  1.0f, inity    , 0.0f, 1.0f, 0.0f,   // bot right
				  1.0f, inity + b, 0.0f, 1.0f, 1.0f,   // top right
				 -1.0f, inity + b, 0.0f, 0.0f, 1.0f }; // top left

		indices = { 0, 1, 2, 0, 2, 3 };

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, rect.size() * sizeof(GL_FLOAT), &rect[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GL_SHORT), &indices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);

		//if (texture_use)
		{
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			stbi_set_flip_vertically_on_load(1);

			int width, height, numchan;
			unsigned char* data = stbi_load(texpath, &width, &height, &numchan, 4);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Unable to load texture\n";
			}
			stbi_image_free(data);
		}
		sh.init(vsp, fsp);
		sh.use();

	}
	void draw()
	{
		glm::mat4 model(1.0f);
		sh.setMat4("model", model);
		glBindVertexArray(vao);
		glBindTexture(GL_TEXTURE_2D, tex);
		glActiveTexture(GL_TEXTURE0);
		sh.setInt("texture1", 0);	
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
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
	char texturepath_road[] = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\rc.jpg";
	char texturepath_water[] = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\wat.jpg";
	char texturepath_grass[] = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\g1.jpg";

	std::string texturepath_car = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\green_car.png";  // size = 0.2
	std::string texturepath_truck = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\truck.png";  // size = 0.3
	std::string texturepath_mini = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\mini.png";  // size = 0.15
	std::string texturepath_lambo = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\lambo.png";  // size = 0.2
	std::string texturepath_cop = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\cop.png";  // size = 0.15

	std::string texturepath_wood = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\wood.png";
	std::string texturepath_finlaywith = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\finlaywith.png";
	std::string texturepath_finlaywithout = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\finlaywithout.png";

	std::string texturepath_frogup = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\frog.png";
	std::string texturepath_frogdown = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\frogdown.png";
	std::string texturepath_frogleft = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\frogleft.png";
	std::string texturepath_frogright = "C:\\Users\\sparks\\Pictures\\Saved Pictures\\frogright.png";


	std::vector<std::string> veh_paths, finlay_paths, frog_path;
	veh_paths = { texturepath_car ,texturepath_truck ,texturepath_mini ,texturepath_lambo ,texturepath_cop };
	finlay_paths = { texturepath_finlaywithout, texturepath_finlaywith };
	frog_path = { texturepath_frogup ,texturepath_frogdown ,texturepath_frogleft ,texturepath_frogright };

	float vehicle_sizes[] = { 0.2f, 0.3f, 0.15f, 0.2f, 0.15f };
	float logturt_sizes[] = { 0.3f, 0.4f, 0.3f, 0.2f, 0.3f };
	short vehicle_dir[] = { 1, -1, -1, 1, 1 };
	short logturt_dir[] = { 1, -1, 1, -1, 1 };
	short vehicle_num[] = { 0, 0, 0, 0, 1 };
	short logturt_num[] = { 2, 3, 2, 3, 2 };

	// Initialize elements
	// frog
	frog* f = new frog(vertexShaderpath, fragmentShaderpath, frog_path);
	std::vector<cars*> vehicle_list;
	std::vector<logs_turtles*> logs_list;
	// cars
	for (int i = 0; i < 5; ++i)
	{
		cars* p = new cars(vehicle_sizes[i], i + 1, vertexShaderpath, fragmentShaderpath, veh_paths[i], vehicle_num[i], vehicle_dir[i]);
		vehicle_list.push_back(p);
	}
	// logs and turtles
	for (int i = 0; i < 5; ++i)
	{
		logs_turtles* p = new logs_turtles(logturt_sizes[i], i + 7, vertexShaderpath, fragmentShaderpath, texturepath_wood, logturt_num[i], logturt_dir[i]);
		logs_list.push_back(p);
	}

	// backgrounds
	std::vector<bg_draw*> backgrounds;
	for (int i = 0; i < 13; ++i)
	{
		bg_draw* b;
		if (i == 0)
			b = new bg_draw(i, vertexShaderpath, fragmentShaderpath, texturepath_grass);
		else if (i < 6)
			b = new bg_draw(i, vertexShaderpath, fragmentShaderpath, texturepath_road);
		else if (i == 6)
			b = new bg_draw(i, vertexShaderpath, fragmentShaderpath, texturepath_grass);
		else if (i < 12)
			b = new bg_draw(i, vertexShaderpath, fragmentShaderpath, texturepath_water);
		else
			b = new bg_draw(i, vertexShaderpath, fragmentShaderpath, texturepath_grass);
		backgrounds.push_back(b);
	}

	// top row finish display

	finish* fin = new finish(vertexShaderpath, fragmentShaderpath, finlay_paths);
	fin->draw();

	// draw mr. frog
	f->draw();

	float vel = f->return_speed();
	float currFrameTime, lastFrameTime = 0.0f, deltaTime, fps = 0;
	float waitsec_start;
	float time_till_next_input = 0.0f;
	short press = 0, frog_level = f->return_level(); bool auto_move = 1;
	float cond;

	bool lvl_comp_crit = 0, wait_a_sec = false;
	deltaTime = 0.001f;
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		currFrameTime = lastFrameTime + deltaTime;
		currFrameTime = glfwGetTime();
		deltaTime = currFrameTime - lastFrameTime;
		lastFrameTime = currFrameTime;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		++fps;
		if (currFrameTime - int(currFrameTime) < 0.001)
		{
			std::cout << "required render time [ms] vs available render time @ 60fps- " << 1000 / fps << " : " << 1000 / 60 << std::endl;
			fps = 0;
		}

		// draw backgrounds
		for (int i = 0; i < 13; ++i)
			backgrounds[i]->draw();

		if (wait_a_sec)
		{
			// Does nothing, only game backrounds render. No keystrokes accepted
			if (currFrameTime - waitsec_start > 1)
				wait_a_sec = 0;
		}
		else
		{
			if (press != 3)
			{
				cond = currFrameTime - time_till_next_input;
				if (cond >= 0)
				{
					// to provide a buffer to prevent seamless movement of player
					cond -= 0.025;
					auto_move = 0;
				}
				press = f->processInput(window, deltaTime, cond, f, auto_move, fin);
				if (!press)
				{
					// non-movement key is pressed or frog tries to leave screen
					// Do nothing
				}
				else if (press == 3)
				{
					// Frog crosses over to the last row
					// Do nothing- separate routine used for it
				}
				else
				{
					float auto_dist = 0.14f;
					short currlevel = f->return_level();
					short prevlevel = f->return_prev_level();
					if (currlevel == 6)
						auto_dist = 0.17f;
					else if (currlevel == 7 && currlevel - prevlevel > 0)
						auto_dist = 0.17f;
					else if (currlevel == 5 && currlevel - prevlevel < 0)
						auto_dist = 0.17f;
					time_till_next_input = currFrameTime + auto_dist / vel;
					auto_move = 1;
				}

				if (press != 3)
				{
					// draw cars
					for (auto it : vehicle_list)
						it->draw_cars(deltaTime);

					// draw logs and turtles
					for (auto it : logs_list)
						it->draw_logs_turtles(deltaTime);

					frog_level = f->return_level();
					if (!auto_move)
					{
						if (frog_level > 0 && frog_level < 6)
							press = f->collision_cars(*vehicle_list[frog_level - 1]);
					}
					if (frog_level > 6 && frog_level < 12 && !auto_move)
						press = f->collision_logs_turtles(*logs_list[frog_level - 7], deltaTime);
					else
						f->draw();
				}
			}
			else
			{
				// level completion criteria met, animate frog going into brush
				// or frog dies due to drowning or by being road-kill!

				// reset the cars
				for (int i = 0; i < 5; ++i)
				{
					delete vehicle_list[i];
					cars* p = new cars(vehicle_sizes[i], i + 1, vertexShaderpath, fragmentShaderpath, veh_paths[i], vehicle_num[i], vehicle_dir[i]);
					vehicle_list[i] = p;
				}
				//reset the logs
				for (int i = 0; i < 5; ++i)
				{
					delete logs_list[i];
					logs_turtles* p = new logs_turtles(logturt_sizes[i], i + 7, vertexShaderpath, fragmentShaderpath, texturepath_wood, logturt_num[i], logturt_dir[i]);
					logs_list[i] = p;
				}
				press = 0;
				// reset the frog
				f->reset_frog();

				if (fin->get_numfins() == 5)
					fin = new finish(vertexShaderpath, fragmentShaderpath, finlay_paths);

				wait_a_sec = true;
				waitsec_start = currFrameTime;
			}
		}

		// draw completed columns
		fin->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	for (auto it : vehicle_list)
		delete it;
	for (auto it : logs_list)
		delete it;
	for (auto it : backgrounds)
		delete it;
	delete f;	
	delete fin;
	glfwTerminate();
	return 0;
}