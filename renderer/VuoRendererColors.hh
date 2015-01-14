/**
 * @file
 * VuoRendererColors interface.
 *
 * @copyright Copyright © 2012–2013 Kosada Incorporated.
 * This interface description may be modified and distributed under the terms of the GNU Lesser General Public License (LGPL) version 2 or later.
 * For more information, see http://vuo.org/license.
 */

#ifndef VUORENDERERCOLORS_HH
#define VUORENDERERCOLORS_HH

#include "VuoNode.hh"
#include "VuoRendererNode.hh"

/**
 * Provides colors for rendered items in a composition.
 */
class VuoRendererColors
{
public:
	static VuoRendererColors *getSharedColors(void);
	VuoRendererColors(VuoNode::TintColor tintColor = VuoNode::TintNone,
					  bool isSelected = false,
					  bool isHovered = false,
					  bool isHighlighted = false,
					  qint64 timeOfLastActivity = VuoRendererItem::notTrackingActivity);

	QColor canvasFill(void);

	QColor nodeFill(void);
	QColor nodeFrame(void);
	QColor nodeTitle(void);
	QColor nodeClass(void);

	QColor portFill(void);
	QColor animatedPortFill(void);
	QColor eventBlockingBarrier(void);
	QColor animatedeventBlockingBarrier(void);
	QColor portTitlebarFill(void);
	QColor portTitle(void);

	QColor cableUpper(void);
	QColor cableMain(void);

	static qint64 getVirtualNodeExecutionOrigin(void);
	static qint64 getVirtualFiredEventOrigin(void);
	static qint64 getVirtualFiredEventOriginForAnimationFadePercentage(qreal percentage);
	static qint64 getVirtualPropagatedEventOrigin(void);

	static const int activityAnimationFadeDuration; ///< Time period, in ms, over which a 'Show Events'-mode animation (e.g., for trigger port firing) fades to its minimum alpha level.

private:
	static VuoRendererColors *sharedColors;

	VuoNode::TintColor tintColor;
	bool isSelected;
	bool isHovered;
	bool isHighlighted;
	double currentFadePercentage;

	QColor tint(QColor color, qreal amount = 1.);
	QColor lerpColor(QColor v0, QColor v1, float t);

	qreal getCurrentAlphaForDefault(qreal defaultAlpha);
	static qreal getMinAlphaForDefault(qreal defaultAlpha);
	static qreal getMaxAlphaForDefault(qreal defaultAlpha);

	// 'Show Events' mode node alpha levels (range: 0.0-1.0)
	static const qreal minNodeFrameAndFillAlpha; ///< Minimum alpha level to which a node may fade following an execution while in 'Show Events' mode.
	static const qreal maxNodeFrameAndFillAlpha; ///< Maximum alpha level, assigned to each node during its execution while in 'Show Events' mode.
	static const qreal defaultNodeFrameAndFillAlpha; ///< Alpha level assigned to each node while in non-'Show Events' mode.
	static const qreal defaultCableUpperAndMainAlpha; ///< Alpha level assigned to each cable while in non-'Show Events' mode.
	static const qreal defaultPortFillAlpha; ///< Alpha level assigned to port fill while in non-'Show Events' mode.
	static const int activityFadeDuration; ///< Time period, in ms, over which a component fades to its minimum alpha level following activity (e.g., node execution) while in 'Show Events' mode.
};

#endif // VUORENDERERCOLORS_HH