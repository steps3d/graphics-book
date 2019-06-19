#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class MyGlWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
	
public:
    MyGlWidget(QWidget *parent = 0) : QOpenGLWidget(parent) {}

protected:
    void initializeGL ()
    {
		initializeOpenGLFunctions ();
    }

    void resizeGL (int w, int h )
    {
	
    }

    void paintGL ()
    {
        glClear ( GL_COLOR_BUFFER_BIT );
    }
};