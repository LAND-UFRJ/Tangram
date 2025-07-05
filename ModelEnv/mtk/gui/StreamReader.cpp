#include "StreamReader.h"

#include <unistd.h>
#include <errno.h>

#include <qapplication.h>
#include <qmessagebox.h>

StreamReader::StreamReader( int descriptor, StreamType type,
                            QWidget *main_window )
{
    this->descriptor  = descriptor;
    this->type        = type;
    this->main_window = main_window;
}

void StreamReader::run()
{
    // No need to flush stderr
    if( type != stderr )
    {
        StreamFlusher *flusher = new StreamFlusher();
        flusher->start();
    }

    /* Initializing buffer */
    bzero( buffer, BUFFER_SIZE );

    while( true )
    {
        errno = 0;
        if( read( descriptor, buffer, BUFFER_SIZE ) == -1 )
        {
            if( QMessageBox::critical( 0,
                                       "Reading Error",
                                       QString( "Could not read output stream. Errno = " + QString::number( errno ) ),
                                       QMessageBox::Abort,
                                       QMessageBox::NoButton ) == QMessageBox::Abort )
            {
                QApplication::exit( 1 );
            }
            
            
        }
        else
        {
            if( strlen( buffer ) > 0 )
            {
                QApplication::postEvent( main_window,
                                         new UpdateStreamEvent( QString( buffer ),
                                                                type ) );
            }

            bzero( buffer, BUFFER_SIZE );
        }
    }
}

/***************************
 * class StreamFlusher     *
 ***************************/
StreamFlusher::StreamFlusher( void )
{
}

void StreamFlusher::run( void )
{
    while( true )
    {
        usleep( UPDATE_INTERVAL );
        fflush( stdout );
    }
}

/***************************
 * class UpdateStreamEvent *
 ***************************/
UpdateStreamEvent::UpdateStreamEvent( QString text,
                                      StreamReader::StreamType type )
                  :QCustomEvent( UPDATE_STREAM_EVENT )
{
    this->text = text;
    this->type = type;
}

QString UpdateStreamEvent::getText( void )
{
    return text;
}

StreamReader::StreamType UpdateStreamEvent::getType( void )
{
    return type;
}
