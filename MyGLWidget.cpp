// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;      
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget() {
}

void MyGLWidget::toggleCamera()
{
  camPlanta = !camPlanta;
  viewTransform();
  projectTransform();
}

void MyGLWidget::iniEscena()
{
  creaBuffersPatricio();
  creaBuffersTerra();
  creaBuffersCub();

  // Paràmetres de l'escena -  ben calculats
  centreEsc = glm::vec3 (0, 1.5, 0);
  radiEsc = sqrt(10*10+10*10+1.5*1.5);

  colFoc = glm::vec3(1,1,1);
  enviaColFocus();
  cubData[0].dimension = 2;
  cubData[0].angle = 0;
  cubData[1].dimension = 2.5;
  cubData[1].angle = 2 * M_PI / 3;
  cubData[2].dimension = 3;
  cubData[2].angle = 4 * M_PI / 3;

  patricio.dimension = 2;
  patricio.angle = -M_PI/2;

  pintaCubs = true;

  colFocus = glm::vec3(1.0, 1.0, 1.0);
  glUniform3fv(colfocusLoc, 1, &colFocus[0]);

  angleCamera = M_PI/2;
  camPlanta = false;

  rotacio = 0;
}

void MyGLWidget::paintGL ()   // Mètode que has de modificar
{
  // En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);
  
  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Pintem el terra
  glBindVertexArray (VAO_Terra);
  modelTransformTerra ();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Pintem el Patricio
  if (!pintaCubs)
  {
    glBindVertexArray (VAO_Patr);
    modelTransformPatricio (patricio.dimension, patricio.angle + rotacio);
    glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);
  }
  else
  {
    for (int i = 0; i < nCubs; ++i)
    {
      glBindVertexArray(VAO_Cub);
      modelTransformCub (cubData[i].dimension, cubData[i].angle + rotacio);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
  }

  glBindVertexArray(0);
}

void MyGLWidget::modelTransformCub (float escala, float angle) 
{
  glm::mat4 TG(1.0f);

  TG = glm::rotate(TG, angle, glm::vec3(0, 1, 0));
  TG = glm::translate(TG, glm::vec3(radi, 0, 0));
  TG = glm::scale(TG, glm::vec3(escala/initCubDimension, escala/initCubDimension, escala/initCubDimension));

  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformPatricio (float escala2, float angle)    // Mètode que has de modificar
{
  glm::mat4 TG(1.0f);

  TG = glm::rotate(TG, angle, glm::vec3(0, 1, 0));
  TG = glm::translate(TG, glm::vec3(0, 0, -radi));
  TG = glm::scale(TG, glm::vec3(escala2, escala2, escala2));

  TG = glm::scale(TG, glm::vec3 (escala, escala, escala));
  TG = glm::translate(TG, -centreBasePat);

  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::viewTransform ()    // Mètode que has de modificar
{
  if (!camPlanta)
    ExamGLWidget::viewTransform();
  else
  {
    glm::vec3 obs, up, VRP;
    obs = glm::vec3(0, 2*radiEsc, 0);
    up = glm::vec3(1, 0, 0);
    VRP = glm::vec3(0, 0, 0);
    View = glm::lookAt(obs, VRP, up);
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
  }
}

void MyGLWidget::projectTransform ()
{
  if (!camPlanta)
    ExamGLWidget::projectTransform();
  else
  {
    glm::mat4 Proj;
    if (ra >= 1) Proj = glm::ortho(-15.0f*ra, 15.0f*ra, -15.0f, 15.0f, zn, zf);
    else Proj = glm::ortho(-15.0f, 15.0f, -15.0f/ra, 15.0f/ra, zn, zf);
    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
  }
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) {
  makeCurrent();
  switch (event->key()) {
  case Qt::Key_V: {
      pintaCubs = !pintaCubs;
    break;
	}
  case Qt::Key_1: {
      patricio.dimension = cubData[0].dimension;
      patricio.angle = -M_PI/2;
    break;
	}
  case Qt::Key_2: {
      patricio.dimension = cubData[1].dimension;
      patricio.angle = -M_PI/2 + cubData[1].angle;
    break;
	}
  case Qt::Key_3: {
      patricio.dimension = cubData[2].dimension;
      patricio.angle = -M_PI/2 + cubData[2].angle;
    break;
	}
  case Qt::Key_F: {
      if (colFocus == glm::vec3(1.0, 1.0, 1.0)) colFocus = glm::vec3(1.0, 1.0, 0.0);
      else colFocus = glm::vec3(1.0, 1.0, 1.0);
      glUniform3fv(colfocusLoc, 1, &colFocus[0]);
    break;
	}
  case Qt::Key_C: {
      toggleCamera();
    break;
	}
  case Qt::Key_Right: {
      rotacio -= 2*M_PI/3;
    break;
	}
  case Qt::Key_Left: {
      rotacio += 2*M_PI/3;
    break;
	}
  case Qt::Key_R: {
      // ...
    break;
	}
  default: ExamGLWidget::keyPressEvent(event); break;
  }
  update();
}

