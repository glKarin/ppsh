import QtQuick 1.1
import com.nokia.meego 1.0

SelectionDialog {
  id: monolog
  property string context
  model: ListModel {}

  onAccepted: { playControl.itemSelected(context, selectedIndex); monolog.destroy() }
  onRejected: { playControl.itemSelected(context, -1);monolog.destroy() }
}
