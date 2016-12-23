#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "vmath.h"

#include "object.h"
#include "sb6ktx.h"
#include "shader.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BumpMappingApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
  
  void loadShaders();
  
  gl::GlslProgRef mGlsl;
  
  struct {
    GLuint mvMatrix;
    GLuint projMatrix;
    GLuint lightPos;
  } uniforms;
  
  struct {
    GLuint color;
    GLuint normals;
  } textures;
  
  sb6::object object;
};

void BumpMappingApp::loadShaders()
{
  mGlsl = gl::GlslProg::create(loadResource("bumpmapping.vs.glsl"),
                               loadResource("bumpmapping.fs.glsl"));

  auto pid = mGlsl->getHandle();
  
  mGlsl->bind();
  
  // note: "location" is actually the texture unit number
  // that glsl will map data to hardware to
  // this is equivalent to using layout (binding = N) in glsl
  // fragment uniforms
  glUniform1i(glGetUniformLocation(pid, "tex_color"), 0);
  glUniform1i(glGetUniformLocation(pid, "tex_normal"), 1);
  
  uniforms.mvMatrix = glGetUniformLocation(pid, "mv_matrix");
  uniforms.projMatrix = glGetUniformLocation(pid, "proj_matrix");
  uniforms.lightPos = glGetUniformLocation(pid, "light_pos");
  
  glActiveTexture(GL_TEXTURE0);
  textures.color = sb6::ktx::file::load("/Users/jmarinelli/Documents/cpp/sb6code/bin/textures/ladybug_co.ktx");
  glActiveTexture(GL_TEXTURE1);
  textures.normals = sb6::ktx::file::load("/Users/jmarinelli/Documents/cpp/sb6code/bin/textures/ladybug_nm.ktx");
  
  object.load("/Users/jmarinelli/Documents/cpp/sb6code/bin/objects/ladybug.sbm");
}

void BumpMappingApp::setup()
{
  loadShaders();
}

void BumpMappingApp::mouseDown( MouseEvent event )
{
}

void BumpMappingApp::update()
{
}

void BumpMappingApp::draw()
{
  static const GLfloat zeros[] = { 0.f, 0.f, 0.f, 0.f };
  static const GLfloat gray[] = {0.1f, 0.1f, 0.1f, 0.0f };
  static const GLfloat ones[] = { 1.f };
  static double lastTime = 0.f;
  static double totalTime = 0.f;
  
  auto elapsed = getElapsedSeconds();
  totalTime = elapsed;
  lastTime = elapsed;
  
  const float f = (float) totalTime;
  
  glClearBufferfv(GL_COLOR, 0, gray);
  glClearBufferfv(GL_DEPTH, 0, ones);
  
  glViewport(0, 0, getWindowWidth(), getWindowHeight());
  glEnable(GL_DEPTH_TEST);
  
  mGlsl->bind();
  
  CameraPersp cam{};
  cam.setPerspective(50.f, getWindowAspectRatio(), 0.1f, 1000.f);
  auto projMatrix = cam.getProjectionMatrix();
  
  glUniformMatrix4fv(uniforms.projMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix));
  
  
  auto translateMat = glm::translate(glm::vec3{0.f, -0.2f, -5.5f});
  auto rotateMat1 = glm::rotate(14.5f, glm::vec3{1.f, 0.f, 0.f});
  auto rotateMat2 = glm::rotate(-20.f, glm::vec3{0.f, 1.f, 0.f});
  
  auto mvMatrix = translateMat * rotateMat1 * rotateMat2 * glm::mat4{};
  
  glUniformMatrix4fv(uniforms.mvMatrix, 1, GL_FALSE, glm::value_ptr(mvMatrix));
  glUniform3fv(uniforms.lightPos, 1, glm::value_ptr(glm::vec3{40.f * sinf(f), 30.f + 20.f * cosf(f), 40.f}));
  object.render();
}

CINDER_APP( BumpMappingApp, RendererGl )
