$returntype $name($parameters)
{
  static $returntype (*func)($parameters);

  if (in_rt)
  {
    printf("$name() is called while in rt section\n");
#if ABORT_ON_VIOLATION
    abort();
#endif
  }
  if(!func)
    func = ($returntype (*)($parameters)) dlsym(RTLD_NEXT, "$name");
  if(!func)
  {
    fprintf(stderr, "Error dlsym'ing $name\n");
    abort();
  }
  return(func($parameternames));
}

