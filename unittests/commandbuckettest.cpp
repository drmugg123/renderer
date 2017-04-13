#include "commandbuckettest.hpp"

using namespace std;

SCENARIO("CommandBucket can allocate and return drawcalls", "[CommandBucket]") {
	GIVEN("that we create an uint32 commandbucket") {
    CommandBucket<int> bucket(2048);

		WHEN("we create a new drawcall") {
      std::shared_ptr<Draw> call = bucket.AddCommand(1u);

			THEN("the drawcall is allocated memory") {
				REQUIRE(call != nullptr);
			}
      THEN("testing things!") {
				GLContextParam param(3, 0, false, false);
				SDL2Window window;
        REQUIRE(window.initialize(param));

        GLShaderProgram program;

        REQUIRE(program.Load("vs.glsl", "fs.glsl"));

        glGenVertexArrays(1, &call->vaoID);
        glBindVertexArray(call->vaoID);

        REQUIRE(glIsVertexArray(call->vaoID));

        glGenBuffers(1, &call->vboID);

        static const GLfloat triangle[] = {
        	-1.0f, -1.0f, 0.0f,
        	 1.0f, -1.0f, 0.0f,
        	 0.0f,  1.0f, 0.0f,
        };

        glBindBuffer(GL_ARRAY_BUFFER, call->vaoID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

        REQUIRE(glIsBuffer(call->vboID));

				program.Use();

			 	glBindBuffer(GL_ARRAY_BUFFER, call->vboID);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(
					 0,
					 3,
					 GL_FLOAT,
					 GL_FALSE,
					 0,
					 (void*)0
				);

				clock_t begin = clock();
				double diff = 0;

        auto onWindowExit = [&]() {
          window.deinitialize();
        };
        auto onWindowResized = [&](int width, int height) {
          std::cout << "window resized to " << width << "x" << height << std::endl;
        };
        auto onWindowMoved = [&](int x, int y) {
          std::cout << "window moved to " << x << "." << y << std::endl;
        };

				glClearColor(0.f, 0.0f, 0.0f, 1.0f);
				while(window.is_initialized()) {
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glDrawArrays(GL_TRIANGLES, 0, 3);

					window.Display();
          window.consume_event(onWindowExit, onWindowResized, onWindowMoved);
				}

				glDisableVertexAttribArray(0);

				glDeleteBuffers(1, &call->vboID);
				glDeleteVertexArrays(1, &call->vaoID);
      }
		}
	}
}