extern void h_init_seed( struct particles *host, int num );

extern void copy_particles_host_to_device( struct particles *host, struct particles *device, int num );

extern void copy_particles_device_to_host( struct particles *host, struct particles *device, int num );

// allocate memory for num particles on host
extern struct particles *h_init_particle_mem( int num );

// allocate memory for num particles on device
extern struct particles *d_init_particle_mem( int num );

// free particle memory on host
extern void h_free_particle_mem( struct particles *list );

// free particle memory on device
extern void d_free_particle_mem( struct particles *list );
