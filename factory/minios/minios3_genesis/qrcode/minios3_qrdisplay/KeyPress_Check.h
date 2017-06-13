#ifndef _AAT_KEYPRESSTEST_
#define _AAT_KEYPRESSTEST_

bool allKeyPressed_gt_twice();
int key_match_func(void* data);
int key_match_func(int key);

void inc_keyPressed(void* data);    // Increase the number of each keys pressed
void inc_keyReleased(void* data);    // Increase the number of each keys released

/*
 * The number of times the button has been clicked
 */
int get_num_button_clicked(int button); 
#endif
