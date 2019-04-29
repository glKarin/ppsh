#include <unistd.h>

#include <gio/gio.h>
#include "test.h"
#include <glib.h>
#include <glib-object.h>
#include "qdir.h"
#include "qfile.h"
#include "qurl.h"
#include "../src/io.h"

int main() {
    g_type_init();
    {
        TEST_INFO (Test attributes on "http://www.example.com/index.html");
        QUrl u ("http://www.example.com/index.html");
        TEST_VERIFY (u.scheme() == "http" );
        TEST_VERIFY (u.path() == "/index.html" );
    }
    {
        TEST_INFO (Test common schemes);
        QUrl u ("mms://www.example.com:8080");
        TEST_VERIFY (u.scheme() == "mms" );
        u = QUrl ("rtsp://www.example.com");
        TEST_VERIFY (u.scheme() == "rtsp" );
        u = QUrl ("https://www.example.com");
        TEST_VERIFY (u.scheme() == "https" );
        u = QUrl ("file:///tmp");
        TEST_VERIFY (u.scheme() == "file" );
    }
    {
        TEST_INFO (Test attributes of urltest.cpp);
        QUrl u (QUrl::fromLocalFile (QDir ("/home/koos/kmplayer/trunk/test/urltest.cpp").absolutePath ()));
        TEST_VERIFY (u.scheme() == "file" );
        TEST_VERIFY (u.toString() == "file:///home/koos/kmplayer/trunk/test/urltest.cpp" );
        TEST_VERIFY (u.path() == "/home/koos/kmplayer/trunk/test/urltest.cpp" );
        TEST_VERIFY (MimeType::findByURL (u).startsWith( "text/"));
    }
    {
        TEST_INFO (Test HTML mimetype);
        QFile f ("bar.txt");
        f.open (QIODevice::WriteOnly);
        f.write ("<html><body>Hi there</body></html>", 34);
        f.close ();
        TEST_VERIFY (MimeType::findByURL (QUrl::fromLocalFile (QFileInfo (f).absoluteFilePath())) == "text/html");
        unlink ("bar.txt");
    }
    {
        TEST_INFO (Test /bin/ls mimetype);
        QFile f ("/bin/ls");
        TEST_VERIFY (MimeType::findByURL (QUrl (f.fileName ())).startsWith ("application/"));
    }
    {
        TEST_INFO (Test /home/koos/MyDocs/.videos/example.avi attributes);
        QUrl u = QUrl::fromLocalFile ("/home/koos/MyDocs/.videos/example.avi");
        TEST_VERIFY (MimeType::findByURL (u).startsWith ("video/"));
        TEST_VERIFY (u.scheme() == "file" );
    }
    {
        TEST_INFO (Test QUrl::encode_string "this is a test");
        TEST_VERIFY (QUrl::toPercentEncoding ("this is a test") == "this%20is%20a%20test");
        TEST_VERIFY (QUrl::fromPercentEncoding ("this%20is%20a%20test") == "this is a test");
    }
    {
        TEST_INFO (Test index.php relative to "http://www.example.com/index.html");
        QUrl u (QUrl ("http://www.example.com/index.html").resolved (QUrl ("index.php")));
        TEST_VERIFY (u.toString () == "http://www.example.com/index.php");
    }
    {
        TEST_INFO (Test "/index.php" relative to "http://www.example.com");
        QUrl u (QUrl ("http://www.example.com").resolved (QUrl ("/index.php")));
        TEST_VERIFY (u.toString () == "http://www.example.com/index.php");
    }
    {
        TEST_INFO (Test index.php relative to "http://www.example.com/");
        QUrl u (QUrl ("http://www.example.com/").resolved (QUrl ("index.php")));
        TEST_VERIFY (u.toString () == "http://www.example.com/index.php");
    }
    {
        TEST_INFO (Test index.php relative to "http://www.example.com/data/");
        QUrl u (QUrl ("http://www.example.com/data/").resolved (QUrl ("index.php")));
        TEST_VERIFY (u.toString () == "http://www.example.com/data/index.php");
    }
    {
        TEST_INFO (Test "/index.php" relative to "http://www.example.com/data/");
        QUrl u (QUrl ("http://www.example.com/data/").resolved(QUrl("/index.php")));
        TEST_VERIFY (u.toString () == "http://www.example.com/index.php");
    }
    {
        TEST_INFO (Test "http://www.example.com/index.php" relative to "http://www.example.com/index.html");
        QUrl u (QUrl ("http://www.example.com/index.html").resolved (QUrl ("http://www.example.com/index.php")));
        TEST_VERIFY (u.toString () == "http://www.example.com/index.php");
    }
    return 0;
}
