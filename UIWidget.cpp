#include "UIWidget.h"

UIWidget* GetUIWidget(SexyString name) {
	typedef UIWidget* (*getWidget)(SexyString*);
	return ((getWidget)getActualOffset(0x270228))(&name);
}

void UIWidget::SetEnabled(bool enabled) {
	typedef void (*setEnabled)(UIWidget*, bool);
	((setEnabled)getActualOffset(0x270E98))(this, enabled);
}