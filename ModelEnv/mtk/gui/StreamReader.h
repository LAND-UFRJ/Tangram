#ifndef STREAMREADER_H
#define STREAMREADER_H

#include <qthread.h>
#include <qevent.h>

const int UPDATE_STREAM_EVENT = QEvent::User + 1;

class StreamReader: public QThread
{
    public:
        typedef enum
        {
            stdout = 1,
            stderr = 2
        } StreamType;
    
        void run( void );        
    
        StreamReader( int, StreamType, QWidget * );

    private:
        static const int BUFFER_SIZE = 512;
        
        int         descriptor;
        StreamType  type;
        QWidget    *main_window;

        char        buffer[ BUFFER_SIZE ];
};

class UpdateStreamEvent: public QCustomEvent
{
    private:
        QString                  text;
        StreamReader::StreamType type;

    public:
        QString                  getText( void );
        StreamReader::StreamType getType( void );

        UpdateStreamEvent( QString, StreamReader::StreamType );
};

class StreamFlusher: public QThread
{
    private:
        static const int UPDATE_INTERVAL = 500000; /* in usec */

    public:
        void run( void );

        StreamFlusher( void );
};

#endif // STREAMREADER_H
