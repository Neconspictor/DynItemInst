import colorama
# Same as add_hook(always=True)
import colored_traceback.always

import pywinauto

from pywinauto.application import Application
import time


# Run a target application
app = Application().start("C://Development//Repositories//NecPack//workspace//_work//tools//VDFS//GothicVDFS.exe")
# Select a menu item
app.Dialog.menu_select("(Builder)")
#app.Dialog.print_control_identifiers()
openScriptButton = app.Dialog.child_window(title="&Open Script", class_name="Button")
openScriptButton.click()


folderSelector = app.OpenScript
#folderSelector.print_control_identifiers()

#child_window(title="Address: Documents", class_name="ToolbarWindow32")
toolbars = folderSelector.children(class_name="ToolbarWindow32")

for elem in toolbars:
    print(elem)


title = toolbars[2].texts()[0]
print(title)

pathAddressToolbar = folderSelector.child_window(title= title, class_name="ToolbarWindow32") #toolbars[2].texts()
pathAddressToolbar.click()

folderSelector.type_keys('C:/Development/Repositories/NecPack/workspace/demo{ENTER}',with_spaces = True)


comboBoxes = folderSelector.children(class_name="ComboBoxEx32")
print(len(comboBoxes))
for elem in comboBoxes:
    print(elem)

#folderSelector = app.OpenScript
#folderSelector.print_control_identifiers()

#fileNameInputField = folderSelector.FileName

fileNameComboBox = folderSelector.child_window(title= comboBoxes[0].texts()[0], class_name="ComboBoxEx32") #child_window(title="File &name:", class_name="Static")
#fileNameInputField.print_control_identifiers()
#child_window(title="File &name:", class_name="Static")

fileNameComboBox.click_input()
fileNameComboBox.type_keys('neclib-demo.vm',with_spaces = True)



buttons = folderSelector.children(class_name="Button")
print(len(buttons))
for elem in buttons:
    print(elem)

openButton = folderSelector.child_window(title= '&Open', class_name="Button") 

openButton.click()


buttons = app.Dialog.children(class_name="Button")
print(len(buttons))
for elem in buttons:
    print(elem)

buildVolumeButton = app.Dialog.child_window(title= '&Build Volume', class_name="Button") 
buildVolumeButton.click()


activeWindow = app.windows(active_only=True)[0]

while (activeWindow.texts()[0] != 'MOD Builder Info' and activeWindow.texts()[0] != 'Error'):
    time.sleep(1)
    activeWindow = app.windows(active_only=True)[0]


window = app.windows(active_only=True)[0]
okButton = window.children(class_name="Button")[0]
okButton.click()

exitButton = app.Dialog.child_window(title= 'E&xit', class_name="Button") 
exitButton.click()