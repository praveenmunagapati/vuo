﻿/**
 * @file
 * vuo.math.cos node implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "node.h"

VuoModuleMetadata({
					  "title" : "Calculate Cosine",
					  "keywords" : [ "trigonometry", "angle", "triangle" ],
					  "version" : "1.0.0",
					  "node": {
						  "isInterface" : false
					  }
				  });

void nodeEvent
(
		VuoInputData(VuoReal) angle,
		VuoOutputData(VuoReal) cosine
)
{
	double radians = angle * M_PI / 180.;
	*cosine = cos(radians);
}
