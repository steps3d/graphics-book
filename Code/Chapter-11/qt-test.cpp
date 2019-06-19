#include <QApplication>
#include "MyGlWidget.h"

int main ( int argc, char **argv )
{
    QApplication   app ( argc, argv );
    QSurfaceFormat format;
    MyGlWidget     widget;
	
	
	format.setRenderableType    ( QSurfaceFormat::OpenGL);
    format.setDepthBufferSize   ( 24 );
    format.setStencilBufferSize ( 8 );
    format.setVersion           ( 3, 3 );
    format.setProfile           ( QSurfaceFormat::CoreProfile );
					  
	widget.setFormat ( format );
    widget.show      ();

    return app.exec();
}