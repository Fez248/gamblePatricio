#include "ExamGLWidget.h"

#define nCubs 3
#define initCubDimension 0.5
#define radi 5

struct cub
{
  float dimension;
  float angle;
};

class MyGLWidget:public ExamGLWidget
{
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : ExamGLWidget(parent) {}
    ~MyGLWidget();

  protected:
    virtual void paintGL ();
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void modelTransformCub (float escala, float angle);
    virtual void modelTransformPatricio (float escala2, float angle);
    virtual void projectTransform ();
    virtual void viewTransform ();
    virtual void iniEscena();

  private:
    int printOglError(const char file[], int line, const char func[]);
    void toggleCamera();

    cub cubData[nCubs];
    cub patricio;
    bool pintaCubs;

    glm::vec3 colFocus;
    bool isOrtogonal;
    float angleCamera;

    float rotacio;
};
