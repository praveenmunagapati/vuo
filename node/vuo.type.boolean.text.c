/**
 * @file
 * vuo.type.boolean.text node implementation.
 *
 * @copyright Copyright © 2012–2013 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

VuoModuleMetadata({
					 "title" : "Convert Boolean to Text",
					 "description" :
						 "<p>Outputs <i>false</i> if the input is <i>false<i> or <i>true</i> if the input is <i>true</i>.</p> \
						 <p>Boolean values can be used for logic. Text can be used for reading and writing.</p> \
						 <p>This node is useful as a type converter to connect ports with different data types.</p>",
					 "keywords" : [ ],
					 "version" : "1.0.0",
					 "node": {
						 "isInterface" : false
					 }
				 });

void nodeEvent
(
		VuoInputData(VuoBoolean, {"default":false}) boolean,
		VuoOutputData(VuoText) text
)
{
	char *textAsCString = VuoBoolean_stringFromValue(boolean);
	*text = VuoText_make(textAsCString);
	free(textAsCString);
}