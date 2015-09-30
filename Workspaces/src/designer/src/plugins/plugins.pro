TEMPLATE = subdirs

#qtHaveModule(opengl): SUBDIRS += tools/view3d
qtHaveModule(webkitwidgets): SUBDIRS += qwebview
#win32: qtHaveModule(axcontainer): SUBDIRS += activeqt
#qtHaveModule(quickwidgets): SUBDIRS += qquickwidget

SUBDIRS +=      \
    elementcom  \
    singleParam \
    simplecurve \
    q2wmap \
    statictxt\
    rectangle \
    circleangle \
    Ellipseangle\
    #compass
    SingleLine\
    button \
    singlestate \
    staticgraph \
    rollinggraph \
    image\
    reserves\
    codeSource\
    commandbutton \
    table

