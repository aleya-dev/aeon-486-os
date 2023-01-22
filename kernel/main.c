static char *video_memory = (char *)0xb8000;

void
kprint (char *str)
{
  char *offset = video_memory;
  while (*str != '\0')
    {
      *offset = *str;
      ++str;
      ++offset;
      ++offset;
    }
}

void
kernel_main (void)
{
  kprint ("Hello from the 32-bit kernel.");
}
