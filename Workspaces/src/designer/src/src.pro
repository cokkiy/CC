TEMPLATE = subdirs

SUBDIRS = \
    uitools \
    lib \
    components \
    designer \
    net
    #net_test

contains(QT_CONFIG, shared): SUBDIRS += plugins

components.depends = lib
designer.depends = components
plugins.depends = lib
net.depends = lib
components.depends = net
plugins.depends = net

qtNomakeTools( \
    lib \
    components \
    designer \
    plugins \
)
