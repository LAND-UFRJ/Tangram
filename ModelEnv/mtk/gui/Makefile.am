lib_LIBRARIES = libmtkgui.a

gui_HEADER =  HMMParameters.h                  \
              HMMActions.h                     \
              HMMEdit.h                        \
              \
              GHMMParameters.h                 \
              GHMMActions.h                    \
              GHMMEdit.h                       \
              \
              HMMBatchParameters.h             \
              HMMBatchActions.h                \
              HMMBatchEdit.h                   \
              \
              HMMBatchVariableParameters.h     \
              HMMBatchVariableActions.h        \
              HMMBatchVariableEdit.h           \
              \
              IntValueParameters.h             \
              IntValueActions.h                \
              IntValueEdit.h                   \
              \
              FloatValueParameters.h           \
              FloatValueActions.h              \
              FloatValueEdit.h                 \
              \
              MainWindow.h                     \
              CreateObject.h                   \
              Matrix.h                         \
              MatrixEditor.h                   \
              ObjectsCombo.h                   \
              QBDParameters.h                  \
              StreamReader.h                   \
              ImportFromTangram.h

SOURCES = ${gui_HEADER}                      \
          HMMParameters.cpp                  \
          HMMActions.cpp                     \
          HMMEdit.cpp                        \
          \
          GHMMParameters.cpp                 \
          GHMMActions.cpp                    \
          GHMMEdit.cpp                       \
          \
          HMMBatchParameters.cpp             \
          HMMBatchActions.cpp                \
          HMMBatchEdit.cpp                   \
          \
          HMMBatchVariableParameters.cpp     \
          HMMBatchVariableActions.cpp        \
          HMMBatchVariableEdit.cpp           \
          \
          IntValueParameters.cpp             \
          IntValueActions.cpp                \
          IntValueEdit.cpp                   \
          \
          FloatValueParameters.cpp           \
          FloatValueActions.cpp              \
          FloatValueEdit.cpp                 \
          \
          MainWindow.cpp                     \
          CreateObject.cpp                   \
          Matrix.cpp                         \
          MatrixEditor.cpp                   \
          ObjectsCombo.cpp                   \
          QBDParameters.cpp                  \
          StreamReader.cpp                   \
          ImportFromTangram.cpp              \
          \
          moc_MainWindow.cpp                 \
          moc_CreateObject.cpp               \
          moc_QBDParameters.cpp              \
          moc_MatrixEditor.cpp               \
          moc_ImportFromTangram.cpp          \
          \
          moc_HMMParameters.cpp              \
          moc_HMMActions.cpp                 \
          moc_HMMEdit.cpp                    \
          \
          moc_GHMMParameters.cpp             \
          moc_GHMMActions.cpp                \
          moc_GHMMEdit.cpp                   \
          \
          moc_HMMBatchParameters.cpp         \
          moc_HMMBatchActions.cpp            \
          moc_HMMBatchEdit.cpp               \
          \
          moc_HMMBatchVariableParameters.cpp \
          moc_HMMBatchVariableActions.cpp    \
          moc_HMMBatchVariableEdit.cpp       \
          \
          moc_IntValueParameters.cpp         \
          moc_IntValueActions.cpp            \
          moc_IntValueEdit.cpp               \
          \
          moc_FloatValueParameters.cpp       \
          moc_FloatValueActions.cpp          \
          moc_FloatValueEdit.cpp
         

libmtkgui_a_SOURCES = $(SOURCES)

libmtkgui_a_CPPFLAGS = -Wno-non-virtual-dtor -DQT_THREAD_SUPPORT \
                       -I../libmtk -I$(QTDIR)/include

MOC = $(QTDIR)/bin/moc

moc_%.cpp: %.h
	$(MOC) -p . -o $@ $<
