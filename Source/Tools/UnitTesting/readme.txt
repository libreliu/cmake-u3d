U3D project

Unit testing development


I.  Test development

a.       Make work directory for test.
b.       Create main header and cpp-files according to templates.
c.       Launch executable utgui.exe and provide next parameters:

            i.      Directory - Test work directory.
            ii.     Test name - must coincide with main header's name.
            iv.     Timeout - number of seconds given to test.
            iii.    Executions - number of times test will be performed.
            v.      Test categories (choose at least one from the listbox).

d.    Perform local development using generated vcproj file.
e.    Check-in the test to Perforce in directory
            //depot/CWG/C3D/Source/Tests/UnitTesting/$CATEGORY/$TESTNAME/, where

            i.      $CATEGORY is one of the categories choosen in GUI.
            ii.     $TESTNAME is the test name choosen in GUI.

II. Suite development

a.       To make suite containing all tests in certain category, select this
                    category from the listbox and press "Category suite" button.
b.       To make custom suite, follow the next steps:
            i.      Choose path where the file with suite will be temporarily
                    stored. You can store these files everywhere you want.
                    Suite name will be equal to the filename.
                    (e.g. you store file at "D:/mysuite". At the end of the
                    process you will have "mysuite" suite on the Build Machine)
            ii.     Press "Download" button.
            iii.    Press "Edit" button.
            iv.     Edit downloaded file - choose only needed tests.
            v.      Press "Upload" button.


Notes:

Make sure that you have path to IFXCore.dll (generally $(U3D_SOURCE)Build/u3d )
in your PATH environment variable.