#include "display.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



namespace OpenGLEngine {

  // WINDOW SURFACE MANAGEMENT ---------------------------------------------------
  int Display::width = 512;
  int Display::height = 512;
  int Display::mouseRelX = 0;
  int Display::mouseRelY = 0;


  bool Display::isRunning() {

    return running;

  }

  int Display::getWindowWidth() {

    return Display::width;

  }

  int Display::getWindowHeight() {

    return Display::height;

  }

  void Display::getDisplayResolution(int *x, int *y) {

  }

  void Display::swapBuffers() {

    SDL_GetWindowSize(window, &Display::width, &Display::height);
    glViewport(0, 0, Display::width, Display::height);
    SDL_GL_SwapWindow(window);

  }
  // ------------------------------------------------

  // INSTANCE & DESTRUCTORS ------------------------------------------------------
  Display* Display::instance = nullptr;

  Display* Display::getInstance() {

    if(instance == nullptr)
      instance = new Display;

    return instance;

  }

  Display::~Display() {

    delete EngineTime::getInstance();
    delete Manager::getInstance();
    SDL_GL_DeleteContext(context);
	  SDL_DestroyWindow(window);
    SDL_Quit();
    delete scene;
    delete framebuffer;

  }
  // -----------------------------------------------------------------------------



  // FUNCTIONS -------------------------------------------------------------------
  void Display::initialize() {

    SDL_Init(SDL_INIT_EVERYTHING);
    #ifdef __linux__
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    #elif __Apple__
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    #endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
  	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);



    window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Display::width, Display::height, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);

    #ifdef __linux__

      glewExperimental=GL_TRUE;
      GLenum status = glewInit();

      if(status != GLEW_OK) {
          return;
      }

      std::cout << "Linux OpenGL Version  :  " << glGetString(GL_VERSION);

    #elif __APPLE__

      std::cout << "Mac OS X OpenGL Version  :  " << glGetString(GL_VERSION);

    #endif

    swapBuffers();
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    std::cout << "ready.\n";
    framebuffer->initialize();
    usingFramebuffer = true;
    scene->initialize();
    numEffects = 1;
    windowTitle = "GLSL TWEAKS  ";
    windowEffect = " [default] ";
    std::stringstream ss;
    ss << EngineTime::Fps();
    windowFps = ss.str();

    EngineTime::getInstance();
    EngineTime::start();

    currentCapturedFrame = 0;
    running = true;
  }

  void Display::update() {

    //Draw To Texture
    if(usingFramebuffer)
      framebuffer->bind();
    else {
      glEnable(GL_DEPTH_TEST);
      glClearColor(0.1, 0.1, 0.1, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    std::stringstream ss;
    ss << EngineTime::Fps();
    windowFps = ss.str();
    SDL_SetWindowTitle(window, std::string(windowTitle + windowFps + windowEffect).c_str());
    scene->render2D(EngineTime::DeltaTime());
    scene->render3D(EngineTime::DeltaTime());

    //Draw texture to screen
    if(usingFramebuffer)
      framebuffer->drawToScreen();


    if(currentCapturedFrame > 0) {
      captureScreenshot();
      currentCapturedFrame--;
    }
    swapBuffers();
    EngineTime::addFrame();
    updateKeyboardAndMouse();

  }

  void Display::captureScreenshot() {

    int width = 512;
    int height = 512;

    int pixels[width][height];
    glReadBuffer(GL_BACK);
    glPixelStoref(GL_UNPACK_ALIGNMENT, 1);
    glReadPixels(0, 0, height, width, GL_RGB, GL_UNSIGNED_BYTE, &pixels[0][0]);
    int temp_num = currentCapturedFrame, digits = 0;
    while (temp_num)
    {
        temp_num /= 10;
        digits++;
    }

    std::stringstream ss;
    while (ss.str().length() + digits != 4)
    {
        ss << "0";
    }
    ss << currentCapturedFrame;
    stbi_write_png(std::string("assets/screenshots/shot").append(ss.str()).append(std::string(".png")).c_str(), height, width, 3, &pixels[0][0], 0);
  }
  // -----------------------------------------------------------------------------



  // 3D RENDERING & INPUT HANDLING -------------------------------------------
  void Display::updateKeyboardAndMouse() {

    SDL_Event e;

    while(SDL_PollEvent(&e)) {

      if(e.type == SDL_QUIT)
        running = false;



      if(e.type == SDL_KEYDOWN ) {

          if(e.key.keysym.sym == SDLK_SPACE) {

            switch (numEffects) {

              case 0:
                framebuffer->reset("assets/shaders/framebuffer/framebuffer");
                windowEffect = " [default] ";
              break;

              case 1:
                framebuffer->reset("assets/shaders/framebuffer_inverted/framebuffer_inverted");
                windowEffect = " [inverted] ";
              break;

              case 2:
                framebuffer->reset("assets/shaders/framebuffer_scanline/framebuffer_scanline");
                windowEffect = " [scanline] ";
              break;

              case 3:
                framebuffer->reset("assets/shaders/framebuffer_blur/framebuffer_blur");
                windowEffect = " [blur] ";
              break;

              case 4:
                framebuffer->reset("assets/shaders/framebuffer_godrays/framebuffer_godrays");
                windowEffect = " [godrays] ";
                numEffects = -1;
              break;

              default:
                numEffects = -1;
              break;

            }
            numEffects++;
            SDL_SetWindowTitle(window, std::string(windowTitle + windowFps + windowEffect).c_str());




          } else if(e.key.keysym.sym == SDLK_RSHIFT) {

            usingFramebuffer = !usingFramebuffer;

          } else if(e.key.keysym.sym == SDLK_RALT) {

            currentCapturedFrame = MAX_CAPTURED_FRAMES;

          }

      }

      scene->interruptKeyboard(e.type, e.key.keysym.sym);
      scene->interruptMouse(e.type, e.button.button);

    }

  }
  // -------------------------------------------------------------------------

}
