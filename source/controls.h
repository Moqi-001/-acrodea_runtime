#ifndef SOLOADER_CONTROLS_H
#define SOLOADER_CONTROLS_H

void controls_init();
void controls_poll();

int so_patch_controls(so_module *mod);

#endif // SOLOADER_CONTROLS_H
