/*
 * event.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef EVENT_H_
#define EVENT_H_
#ifdef __cplusplus
 extern "C" {
#endif

typedef struct {
	uint32_t  e;
	uint32_t  v;
	void     (*Callback)(void);
} event_t;

void event_init(void);
bool event_put(uint32_t e, uint32_t v, void (*callback)(void));
bool event_get(event_t *event, bool remain);
bool event_empty(void);

#ifdef __cplusplus
}
#endif
#endif /* EVENT_H_ */
