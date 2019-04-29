import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
  id: rootWindow
  platformStyle: PageStackWindowStyle { id: defaultStyle;backgroundColor:playControl.videoVisible ? '#080810':'#000000' }

  Component.onCompleted: {
    theme.inverted = true;
  }
  initialPage: MainPage {}
}
