/**
 * @file
 * VuoInputEditorLoopType implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "VuoInputEditorLoopType.hh"

extern "C"
{
	#include "VuoLoopType.h"
}

/**
 * Constructs a VuoInputEditorLoopType object.
 */
VuoInputEditor * VuoInputEditorLoopTypeFactory::newInputEditor()
{
	return new VuoInputEditorLoopType();
}

/**
 * Creates the tree that models the menu.
 */
VuoInputEditorMenuItem * VuoInputEditorLoopType::setUpMenuTree()
{
	VuoInputEditorMenuItem *optionsTree = new VuoInputEditorMenuItem("root");

	for(int i = 0; i < VuoLoopType_None+1; i++)
	{
		json_object *optionAsJson = VuoLoopType_jsonFromValue( (VuoLoopType)i );
		char *optionSummary = VuoLoopType_summaryFromValue( (VuoLoopType)i );
		VuoInputEditorMenuItem *optionItem = new VuoInputEditorMenuItem(optionSummary, optionAsJson);
		optionsTree->addItem(optionItem);
	}

	return optionsTree;
}