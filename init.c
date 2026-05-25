
void init_coder(char **argv, int nb_coders, int i, t_coder **coder)
{
	*coder = malloc(nb_coders * sizeof(t_coder));
	if (!*coder)
		return NULL;
	// still need ot work on the init
	while (i <= nb_coders)
	{
		coder[i]->nb = i;
		coder[i]->priority_rank = 0;
		coder[i]->nb_dongle = 0;
		coder[i]->nb_of_compiles = ft_atoi(argv[6]);
		coder[i]->time_to_burnout = ft_atoi(argv[2]) * 1000; // do i need this here?
		coder[i]->last_time_compiled = get_time_ms();
		coder[i]->l_dongle = &dongle[i];
		coder[i]->r_dongle = &dongle[(i - 1 + nb_coders) % nb_coders];
		coder[i]->sim = &sim;
		coder[i]->args = &args;
		i++;
	}
}

void init_dongle(int i, char **argv, int nb_coders, t_dongle **dongle)
{
	*dongle = malloc(nb_coder * sizeof(t_dongle))
	if (!*dongle)
		return NULL;
	while (i <= nb_coders)
	{
		dongle[i]->rank = i;
		dongle[i]->status = 0;
		dongle[i]->time_to_cooldown = ft_atoi(argv[7]);
		createHeap(&dongle[i].heap, 2);
		if (pthread_mutex_init(&dongle[i]->lock, NULL) != 0)
			return 1;
		pthread_cond_init(&dongle[i]->condDongle, NULL);
		i++;
	}
}

void init_args(char **argv, int nb_coders, t_args **args)
{
	args->nb_coders = nb_coders; // do i need to keep this??
	args->time_to_burnout = ft_atoi(argv[2]) * 1000; 
	args->time_to_compile = ft_atoi(argv[3]);
	args->time_to_debug = ft_atoi(argv[4]);
	args->time_to_refactor = ft_atoi(argv[5]);
}

void init_sim()
{

}

void init_management(char **argv)
{
	int nb_coders;
	int i;

	t_dongle *dongle;
	t_args *args;
	t_coder *coder;

	i = 1;
	nb_coders = ft_atoi(argv[1]);
	init_args(argv, int nb_coders, *args);
	init_dongle(i, *argv, nb_coder, *dongle);
	init_coder(*argv, nb_coder, i, *coder);
}