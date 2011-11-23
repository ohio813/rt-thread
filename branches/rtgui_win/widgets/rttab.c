/*
 * File      : rttab.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * RTTAB = RT-thread TAB widget
 * Change Logs:
 * Date           Author       Notes
 * 2011-05-01     amsl         Frist Version
 */
#include <rtgui/widgets/widget.h>
#include <rtgui/widgets/rttab.h>

static rt_bool_t rtgui_rttab_ppl_onclick (PVOID wdt, rtgui_event_t* event);

static void _rtgui_rttab_constructor(rtgui_rttab_t *tab)
{
	/* init widget and set event handler */
	RTGUI_WIDGET_FLAG(tab) |= RTGUI_WIDGET_FLAG_FOCUSABLE;
	rtgui_widget_set_event_handler(tab, rtgui_rttab_event_handler);
	rtgui_widget_set_style(tab,RTGUI_BORDER_UP);

	tab->caption = RT_NULL;
#if (1) /* debug */
	tab->orient = RTGUI_RTTAB_ORIENT_TOP;
#else
	tab->orient = RTGUI_RTTAB_ORIENT_BOTTOM;
#endif
	tab->tag_size = RTGUI_RTTAB_DEFAULT_TITLE_SIZE;
	tab->focus_color = black; /* please reset it */
	tab->tag_count = 0;
	tab->frist_tag = 0;
	tab->now_tag = 0;
	tab->tags = RT_NULL;
	tab->ppl = RT_NULL;
}

static void _rtgui_rttab_destructor(rtgui_rttab_t *tab)
{
}

rtgui_type_t *rtgui_rttab_type_get(void)
{
	static rtgui_type_t *tab_type = RT_NULL;

	if(!tab_type)
	{
		tab_type = rtgui_type_create("rttab", RTGUI_CONTAINER_TYPE,
			sizeof(rtgui_rttab_t),
			RTGUI_CONSTRUCTOR(_rtgui_rttab_constructor),
			RTGUI_DESTRUCTOR(_rtgui_rttab_destructor));
	}

	return tab_type;
}

rtgui_rttab_t* rtgui_rttab_create(PVOID parent, char* caption,int left, int top, int w, int h)
{
    rtgui_rttab_t* tab;

	RT_ASSERT(parent != RT_NULL);

    tab = rtgui_widget_create(RTGUI_RTTAB_TYPE);
    if(tab != RT_NULL)
    {
		rtgui_rect_t rect;
		int height = 0;
		/* set default rect */
		rtgui_widget_get_rect(parent, &rect);
		rtgui_widget_rect_to_device(parent,&rect);
		rect.x1 += left;
		rect.y1 += top;
		rect.x2 = rect.x1+w;
		rect.y2 = rect.y1+h;
		rtgui_widget_set_rect(tab, &rect);
		rtgui_container_add_child(parent, tab);

		height = rtgui_font_get_font_height(RTGUI_WIDGET_FONT(tab));
		tab->tag_size = height + RTGUI_WIDGET_DEFAULT_MARGIN*2;
    }

    return tab;
}

void rtgui_rttab_destroy(rtgui_rttab_t* tab)
{
	rtgui_widget_destroy(tab);
}

/* draw left/top/right */
void rtgui_rttab_draw_title_rect(struct rtgui_dc* dc, rtgui_rect_t* rect,
							   rtgui_color_t c1, rtgui_color_t c2, rt_uint32_t flag)
{
	RT_ASSERT(dc != RT_NULL);

	RTGUI_DC_FC(dc) = c1;
    rtgui_dc_draw_vline(dc, rect->x1, rect->y1, rect->y2);
	if(flag == RTGUI_RTTAB_ORIENT_TOP)
    {
		rtgui_dc_draw_hline(dc, rect->x1 + 1, rect->x2, rect->y1);
	}

	RTGUI_DC_FC(dc) = c2;

	rtgui_dc_draw_vline(dc, rect->x2 - 1, rect->y1, rect->y2);
	if(flag == RTGUI_RTTAB_ORIENT_BOTTOM)
	{
		rtgui_dc_draw_hline(dc, rect->x1, rect->x2, rect->y2 - 1);
	}
}

void rtgui_rttab_ondraw(rtgui_rttab_t* tab)
{
	int i, _left=0, _right=0;
	rtgui_rect_t rect, tmp_rect;
	rtgui_rttab_item_t* item;
	rtgui_dc_t* dc;

	RT_ASSERT(tab != RT_NULL);

	/* begin drawing */
	dc = rtgui_dc_begin_drawing(tab);
	if(dc == RT_NULL)return;

	rtgui_widget_get_rect(tab, &rect);

	/* draw rttab widget border */
	tmp_rect = rect;
	if(tab->orient==RTGUI_RTTAB_ORIENT_TOP)
	{
		tmp_rect.y2 = tmp_rect.y1 + RTGUI_WIDGET_BORDER(tab) + tab->tag_size;
		RTGUI_DC_FC(dc) = RTGUI_WIDGET_BACKGROUND(tab);
		/* draw tagbar border */
		rtgui_dc_draw_line(dc,tmp_rect.x1, tmp_rect.y1, tmp_rect.x1, tmp_rect.y2);
		rtgui_dc_draw_line(dc,tmp_rect.x2-1, tmp_rect.y1, tmp_rect.x2-1, tmp_rect.y2);
		rtgui_dc_draw_line(dc,tmp_rect.x1, tmp_rect.y1, tmp_rect.x2, tmp_rect.y1);

		tmp_rect.y1 = tmp_rect.y2-1;
		tmp_rect.y2 = rect.y2;
		/* fill client background */
		rtgui_rect_inflate(&tmp_rect,-RTGUI_WIDGET_BORDER(tab));
		rtgui_dc_fill_rect(dc, &tmp_rect);
		rtgui_rect_inflate(&tmp_rect,RTGUI_WIDGET_BORDER(tab));
		/* draw client border */
		RTGUI_DC_FC(dc) = white;
		rtgui_dc_draw_line(dc,tmp_rect.x1, tmp_rect.y1, tmp_rect.x1, tmp_rect.y2-1);
		RTGUI_DC_FC(dc) = dark;
		rtgui_dc_draw_line(dc,tmp_rect.x2-1, tmp_rect.y1, tmp_rect.x2-1, tmp_rect.y2-1);
		rtgui_dc_draw_line(dc,tmp_rect.x1, tmp_rect.y2-1, tmp_rect.x2, tmp_rect.y2-1);
	}
	else if(tab->orient==RTGUI_RTTAB_ORIENT_BOTTOM)
	{
		tmp_rect.y2 = tmp_rect.y1 + (rtgui_rect_height(rect)-tab->tag_size);
		/* fill client background */
		rtgui_rect_inflate(&tmp_rect,-RTGUI_WIDGET_BORDER(tab));
		rtgui_dc_fill_rect(dc, &tmp_rect);
		rtgui_rect_inflate(&tmp_rect,RTGUI_WIDGET_BORDER(tab));
		/* draw client border */
		RTGUI_DC_FC(dc) = white;
		rtgui_dc_draw_line(dc,tmp_rect.x1, tmp_rect.y1, tmp_rect.x1, tmp_rect.y2-1);
		rtgui_dc_draw_line(dc,tmp_rect.x1, tmp_rect.y1, tmp_rect.x2, tmp_rect.y1);
		RTGUI_DC_FC(dc) = dark;
		rtgui_dc_draw_line(dc,tmp_rect.x2-1, tmp_rect.y1, tmp_rect.x2-1, tmp_rect.y2);

		tmp_rect.y1 = tmp_rect.y2-1;
		tmp_rect.y2 = rect.y2;
		/* draw tagbar border */
		RTGUI_DC_FC(dc) = RTGUI_WIDGET_BACKGROUND(tab);
		rtgui_dc_draw_line(dc,tmp_rect.x1, tmp_rect.y1, tmp_rect.x1, tmp_rect.y2-1);
		rtgui_dc_draw_line(dc,tmp_rect.x2-1, tmp_rect.y1+1, tmp_rect.x2-1, tmp_rect.y2-1);
		rtgui_dc_draw_line(dc,tmp_rect.x1, tmp_rect.y2-1, tmp_rect.x2, tmp_rect.y2-1);
	}

	tmp_rect = rect;

	if(tab->orient==RTGUI_RTTAB_ORIENT_TOP)
	{
		for(i=tab->frist_tag; i<tab->tag_count; i++)
		{
			item = &(tab->tags[i]);
			tmp_rect.x2 = tmp_rect.x1 + item->tag_width;
			tmp_rect.y2 = tmp_rect.y1 + RTGUI_WIDGET_BORDER(tab) + tab->tag_size;
			/* fill tagbar background, use same color as client area. */
			RTGUI_DC_BC(dc) = RTGUI_WIDGET_BACKGROUND(item->tag);
			rtgui_dc_fill_rect(dc, &tmp_rect);
			/* draw tagbutton border */
			rtgui_rttab_draw_title_rect(dc, &tmp_rect, white, dark, RTGUI_RTTAB_ORIENT_TOP);
			rtgui_rect_inflate(&tmp_rect, -1);
			rtgui_rttab_draw_title_rect(dc, &tmp_rect, default_background, dark_grey, RTGUI_RTTAB_ORIENT_TOP);
			rtgui_rect_inflate(&tmp_rect, 1);

			if(i == tab->now_tag)
			{
				_left = tmp_rect.x1;
				_right = tmp_rect.x2;
				RTGUI_DC_FC(dc) = tab->focus_color;
			}
			else
			{
				RTGUI_DC_FC(dc) = black;
			}

			if(item->image != RT_NULL)
			{
				rtgui_rect_t image_rect = {0};
				image_rect.x2 = image_rect.x1+item->image->w;
				image_rect.y2 = image_rect.y1+item->image->h;
				rtgui_rect_moveto_align(&tmp_rect, &image_rect, RTGUI_ALIGN_CENTER_VERTICAL);
				rtgui_image_paste(item->image, dc, &image_rect, white);
				tmp_rect.x1 += item->image->w;
			}

			rtgui_dc_draw_text(dc, item->tag->title, &tmp_rect);
			tmp_rect.x1 = tmp_rect.x2;
		}
		tmp_rect.x2 = rect.x2;
		if(RTGUI_WIDGET_PARENT(tab) != RT_NULL)
			RTGUI_DC_BC(dc) = RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET_PARENT(tab));
		else
			RTGUI_DC_BC(dc) = default_background;
		rtgui_dc_fill_rect(dc, &tmp_rect);

		RTGUI_DC_FC(dc) = white;
		rtgui_dc_draw_line(dc,rect.x1, tmp_rect.y2-1, _left, tmp_rect.y2-1);
		rtgui_dc_draw_line(dc,_right, tmp_rect.y2-1, rect.x2-1, tmp_rect.y2-1);
	}
	else if(tab->orient == RTGUI_RTTAB_ORIENT_BOTTOM)
	{
		int h;
		h = rtgui_rect_height(rect)-RTGUI_WIDGET_BORDER(tab)*2 - tab->tag_size;
		tmp_rect.y1 += RTGUI_WIDGET_BORDER(tab) + h;
		tmp_rect.y2 = tmp_rect.y1 + tab->tag_size;

		for(i=tab->frist_tag; i<tab->tag_count; i++)
		{
			item = &(tab->tags[i]);
			tmp_rect.x2 = tmp_rect.x1+item->tag_width;
			tmp_rect.y2 = tmp_rect.y1+tab->tag_size;
			RTGUI_DC_BC(dc) = RTGUI_WIDGET_BACKGROUND(item->tag);
			rtgui_dc_fill_rect(dc, &tmp_rect);
			/* draw tagbutton border */
			rtgui_rttab_draw_title_rect(dc, &tmp_rect, white, dark, RTGUI_RTTAB_ORIENT_BOTTOM);
			rtgui_rect_inflate(&tmp_rect, -1);
			rtgui_rttab_draw_title_rect(dc, &tmp_rect, default_background, dark_grey, RTGUI_RTTAB_ORIENT_BOTTOM);
			rtgui_rect_inflate(&tmp_rect, 1);

			if(i == tab->now_tag)
			{
				_left = tmp_rect.x1;
				_right = tmp_rect.x2;
				RTGUI_DC_FC(dc) = tab->focus_color;
			}
			else
			{
				RTGUI_DC_FC(dc) = black;
			}

			if(item->image != RT_NULL)
			{
				rtgui_rect_t image_rect = {0};
				image_rect.x2 = image_rect.x1+item->image->w;
				image_rect.y2 = image_rect.y1+item->image->h;
				rtgui_rect_moveto_align(&tmp_rect, &image_rect, RTGUI_ALIGN_CENTER_VERTICAL);
				rtgui_image_paste(item->image, dc, &image_rect, white);
				tmp_rect.x1 += item->image->w;
			}

			rtgui_dc_draw_text(dc, item->tag->title, &tmp_rect);
			tmp_rect.x1 = tmp_rect.x2;
		}
		tmp_rect.x2 = rect.x2;
		if(RTGUI_WIDGET_PARENT(tab) != RT_NULL)
			RTGUI_DC_BC(dc) = RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET_PARENT(tab));
		else
			RTGUI_DC_BC(dc) = default_background;
		rtgui_dc_fill_rect(dc, &tmp_rect);

		RTGUI_DC_FC(dc) = dark;
		rtgui_dc_draw_line(dc,rect.x1, tmp_rect.y1, _left, tmp_rect.y1);
		rtgui_dc_draw_line(dc,_right, tmp_rect.y1, rect.x2, tmp_rect.y1);
	}

	rtgui_dc_end_drawing(dc);
}

/* add a new page tag */
rt_err_t rtgui_rttab_add_tag(rtgui_rttab_t* tab, char* name, rtgui_image_t* image)
{
	rtgui_rect_t rect;
	rtgui_rttab_item_t item={0}, *_tags;
	int w, h, _top;

	RT_ASSERT(tab != RT_NULL);

	rtgui_widget_get_rect(tab, &rect);

	w = rtgui_rect_width(rect)-RTGUI_WIDGET_BORDER(tab)*2;
	h = rtgui_rect_height(rect)-RTGUI_WIDGET_BORDER(tab)*2 - tab->tag_size;

	if(tab->orient==RTGUI_RTTAB_ORIENT_TOP)
		_top = RTGUI_WIDGET_BORDER(tab) + tab->tag_size;
	else
	    _top = RTGUI_WIDGET_BORDER(tab);

	item.tag = rtgui_view_create(tab, name, RTGUI_WIDGET_BORDER(tab), _top, w, h);
	if(item.tag == RT_NULL) return;
	RTGUI_WIDGET_HIDE(item.tag);
#if (0)
{	/* debug: set color for each tag */
	#include <stdlib.h> /* use extern function: rand() */
	RTGUI_WIDGET_BACKGROUND(item.tag) = RTGUI_RGB(rand()%255, rand()%255, rand()%255);
}
#endif
	item.image = image;
	item.tag_width = rtgui_font_get_string_width(RTGUI_WIDGET_FONT(tab), name);
	item.tag_width += RTGUI_WIDGET_DEFAULT_MARGIN*2;
	if(item.image != RT_NULL) item.tag_width += item.image->w+RTGUI_WIDGET_DEFAULT_MARGIN;

	if(tab->tag_count == 0)
	{
		_tags = rt_malloc(sizeof(rtgui_rttab_item_t));
	}
	else
	{
		_tags = rt_realloc(tab->tags,sizeof(rtgui_rttab_item_t)*(tab->tag_count+1));
	}

	_tags[tab->tag_count].tag = item.tag;
	_tags[tab->tag_count].image = item.image;
	_tags[tab->tag_count].tag_width = item.tag_width;
	tab->tag_count++;
	tab->tags = _tags;

	RTGUI_WIDGET_UNHIDE(_tags[tab->now_tag].tag);

	{
		int i, uw=0, hide_num=0;
		rtgui_rttab_item_t* tag;

		for(i=0; i<tab->tag_count; i++)
		{
			tag = &(tab->tags[i]);
			uw += tag->tag_width;
			if(uw >= w)
			{
				hide_num = tab->tag_count-i;

				if(tab->ppl != RT_NULL)
				{
					rtgui_widget_get_rect(tab, &rect);
					if((uw-tag->tag_width) >= (w-rtgui_rect_width(rect)))
						hide_num += 1;
				}
				break;
			}
		}
		if(hide_num>0)
		{
			if(tab->ppl == RT_NULL)
			{
				int ppl_top=0;
				int ppl_h = tab->tag_size-2;
				if(tab->orient==RTGUI_RTTAB_ORIENT_TOP)
					ppl_top = tab->tag_size-ppl_h;
				else if(tab->orient==RTGUI_RTTAB_ORIENT_BOTTOM)
					ppl_top = h+RTGUI_WIDGET_BORDER(tab)*2;
				tab->ppl = rtgui_propel_create(tab, rtgui_rect_width(rect)-ppl_h*2,
						ppl_top, ppl_h*2, ppl_h, RTGUI_HORIZONTAL);
				if (tab->ppl == RT_NULL)
				{
					return RT_ENOMEM;
				}
				rtgui_propel_bind(tab->ppl, (rt_uint32_t*)&(tab->frist_tag));
				tab->ppl->wdtlnk = tab;
				tab->ppl->on_click = rtgui_rttab_ppl_onclick;
				tab->ppl->range_max = hide_num;
			}
			else
			{
				tab->ppl->range_max = hide_num;
			}
		}
	}
	return RT_EOK;
}

rtgui_container_t* rtgui_rttab_get_container_by_index(rtgui_rttab_t* tab, rt_int16_t index)
{
	rtgui_rttab_item_t* item;

	if(tab == RT_NULL) return RT_NULL;
	if(tab->tag_count <= 0) return RT_NULL;
	if(index >= tab->tag_count) return RT_NULL;

	item = &(tab->tags[index]);
	return RTGUI_CONTAINER(item->tag);
}

rtgui_container_t* rtgui_rttab_get_container_by_title(rtgui_rttab_t* tab, char* title)
{
	int i;
	rtgui_rttab_item_t* item;

	if(tab == RT_NULL) return RT_NULL;
	if(tab->tag_count <= 0) return RT_NULL;
	if(title == RT_NULL) return RT_NULL;

	for(i=0; i<tab->tag_count; i++)
	{
		item = &(tab->tags[i]);
		if(rt_strstr(title, item->tag->title) != RT_NULL)
		{
			return RTGUI_CONTAINER(item->tag);
		}
	}

	return RT_NULL;
}

static void rtgui_rttab_onmouse(rtgui_rttab_t* tab, rtgui_event_mouse_t* emouse)
{
	rtgui_rect_t rect, tagbar_rect;

	/* get physical extent information */
	rtgui_widget_get_rect(tab, &rect);
	rtgui_widget_rect_to_device(tab, &rect);

	tagbar_rect = rect;
	if(tab->orient==RTGUI_RTTAB_ORIENT_TOP)
	{
		tagbar_rect.y2 = tagbar_rect.y1 + RTGUI_WIDGET_BORDER(tab) + tab->tag_size;
	}
	else if(tab->orient==RTGUI_RTTAB_ORIENT_BOTTOM)
	{
		tagbar_rect.y1 = tagbar_rect.y1 + (rtgui_rect_height(rect)-tab->tag_size);
		tagbar_rect.y2 = rect.y2;
	}

	if(tab->ppl != RT_NULL)
	{	/* cut off propel from tagbar area */
		rtgui_rect_t ppl_rect;
		rtgui_widget_get_rect(tab->ppl, &ppl_rect);
		tagbar_rect.x2 -= rtgui_rect_width(ppl_rect);
	}

	if(rtgui_rect_contains_point(&tagbar_rect, emouse->x, emouse->y) == RT_EOK)
	{   /* on tag bar */
		int i;
		rtgui_rect_t tmp_rect = tagbar_rect;
		rtgui_rttab_item_t* item;

		for(i=tab->frist_tag; i<tab->tag_count; i++)
		{
			item = &(tab->tags[i]);
			tmp_rect.x2 = tmp_rect.x1+item->tag_width;
			if(rtgui_rect_contains_point(&tmp_rect, emouse->x, emouse->y) == RT_EOK)
			{	/* inside a tag */
				if(emouse->button & RTGUI_MOUSE_BUTTON_DOWN)
				{
					if(i != tab->now_tag)
					{
						rtgui_widget_t* top;
						item = &(tab->tags[tab->now_tag]);
						/* hide old item */
						RTGUI_WIDGET_HIDE(item->tag);

						tab->now_tag = i;
						item = &(tab->tags[tab->now_tag]);
						RTGUI_WIDGET_UNHIDE(item->tag);
						top = rtgui_widget_get_toplevel(tab);
						rtgui_widget_update_clip(top);
						rtgui_widget_update(tab);
					}
				}
				return;
			}
			tmp_rect.x1 = tmp_rect.x2;
		}
	}
	else
	{	/* other child widget */
		rtgui_container_dispatch_mouse_event(RTGUI_CONTAINER(tab), emouse);
	}
}

rt_bool_t rtgui_rttab_event_handler(PVOID wdt, rtgui_event_t* event)
{
	rtgui_widget_t *widget = (rtgui_widget_t*)wdt;
	rtgui_rttab_t* tab = (rtgui_rttab_t*)widget;

	if(tab == RT_NULL)return RT_FALSE;

	switch (event->type)
	{
		case RTGUI_EVENT_PAINT:
			if(widget->on_draw != RT_NULL )
				widget->on_draw(widget, event);
			else
			{
				rtgui_rttab_ondraw(tab);
				/* paint on each child */
				rtgui_container_dispatch_event(RTGUI_CONTAINER(tab), event);
			}
			break;
		/* not support keyboard event in current version. */
		/*case RTGUI_EVENT_KBD:
			break;*/

		case RTGUI_EVENT_MOUSE_BUTTON:
			if(widget->on_mouseclick != RT_NULL)
			{
				widget->on_mouseclick(widget, event);
			}
			else
			{
				rtgui_rttab_onmouse(tab, (rtgui_event_mouse_t*)event);
			}
			break;

		default:
			break;
	}

	return RT_FALSE;
}

/* bind widget: propel on click event */
static rt_bool_t rtgui_rttab_ppl_onclick (PVOID wdt, rtgui_event_t* event)
{
	int i;
	rtgui_rttab_t* tab = wdt;
	rtgui_rttab_item_t* item;
	rtgui_widget_t* top;

	for(i=0; i<tab->tag_count; i++)
	{
		item = &(tab->tags[i]);
		if(!RTGUI_WIDGET_IS_HIDE(item->tag))
		{
			RTGUI_WIDGET_HIDE(item->tag);
			item = &(tab->tags[tab->now_tag]);
			RTGUI_WIDGET_UNHIDE(item->tag);
			top = rtgui_widget_get_toplevel(tab);
			rtgui_widget_update_clip(top);
			rtgui_widget_update(tab);
		}
	}

	return RT_TRUE;
}
