import QtQuick 2.2

Rectangle
{
    id: rect

    color: "#282828"

    GridView
    {
        id: gridView
        cellHeight: 150
        cellWidth: 120

        anchors.fill: parent
        Component
        {
            id: contactsDelegate
            MouseArea
            {
                id: mouseArea
                hoverEnabled: true
                height: 100
                width: 100
                Image
                {
                    id: image

                    width: parent.width
                    height: parent.height
                    source: coverPath

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                   states: State {
                       name: "down"
                       when: mouseArea.pressed == true

                       PropertyChanges {
                           target: image
                           width: gridView.cellWidth
                           height: gridView.cellHeight
                       }
                   }

                   transitions: Transition {
                       from: "";
                       to: "down";
                       //reversible: true
                       ParallelAnimation {
                            NumberAnimation { properties: "width,height"; duration: 500; easing.type: Easing.Bezier }
                       }
                   }
                }

                Text
                {
                    id: txt
                    font.bold: true
                    text: albumName
                    color: "white"
                    elide: Text.ElideRight;

                    horizontalAlignment: Text.AlignHCenter
                    anchors.top: image.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right

                    wrapMode: Text.WordWrap
                }


                onClicked: {
                    var idx = gridView.indexAt(x, y);
                    interpreter.setSelectedAlbum( idx );
                }

                onDoubleClicked: {
                    var idx = gridView.indexAt(x, y);
                    interpreter.setDoubleClicked( idx );
                }
            }
        }

        model: myModel
        delegate: contactsDelegate
    }
}

