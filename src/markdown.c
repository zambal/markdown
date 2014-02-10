#include <string.h>
#include <stdint.h>

#include "erl_nif.h"

#include "buffer.h"
#include "markdown.h"
#include "html.h"

#define OUTPUT_UNIT 128
#define MAX_INPUT_SIZE_SHORT_JOB 20000

static int
is_short_job(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifBinary input;
  if (enif_inspect_binary(env, argv[0], &input) == 0) {
    return -1;
  }  
  
  return input.size < MAX_INPUT_SIZE_SHORT_JOB;
}

static ERL_NIF_TERM
do_job(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifBinary input;
  ErlNifBinary output;
  
  ERL_NIF_TERM options;
  ERL_NIF_TERM atom_true;
  ERL_NIF_TERM atom_tables;
  ERL_NIF_TERM atom_autolink;
  ERL_NIF_TERM atom_fenced_code;
  
  ERL_NIF_TERM term;
  const ERL_NIF_TERM* tuple;
  int tuple_size;
  
  unsigned int extensions;

  hoedown_buffer* ob;
  hoedown_markdown* markdown;
  hoedown_renderer* renderer;

  if (enif_inspect_binary(env, argv[0], &input) == 0) {
    return enif_make_badarg(env);
  }

  if (input.size < 1) {
    return argv[0];
  }

  options = argv[1];
  extensions = 0;
  atom_true = enif_make_atom(env, "true");
  atom_tables = enif_make_atom(env, "tables");
  atom_autolink = enif_make_atom(env, "autolink");
  atom_fenced_code = enif_make_atom(env, "fenced_code");

  while (enif_get_list_cell(env, options, &term, &options) != 0) {
    if (enif_get_tuple(env, term, &tuple_size, &tuple) != 0) {
      if (tuple_size > 1) {
        if (enif_compare(tuple[0], atom_tables) == 0) {
          if (enif_compare(tuple[1], atom_true) == 0) {
            extensions |= HOEDOWN_EXT_TABLES;
            continue;
          }
        }

        if (enif_compare(tuple[0], atom_autolink) == 0) {
          if (enif_compare(tuple[1], atom_true) == 0) {
            extensions |= HOEDOWN_EXT_AUTOLINK;
            continue;
          }
        }

        if (enif_compare(tuple[0], atom_fenced_code) == 0) {
          if (enif_compare(tuple[1], atom_true) == 0) {
            extensions |= HOEDOWN_EXT_FENCED_CODE;
            continue;
          }
        }
      }
    }
  }

  ob = hoedown_buffer_new(OUTPUT_UNIT);
  renderer = hoedown_html_renderer_new(0, 0);
  markdown = hoedown_markdown_new(extensions, 16, renderer);
  hoedown_markdown_render(ob, (uint8_t*) input.data, input.size, markdown);

  hoedown_html_renderer_free(renderer);
  hoedown_markdown_free(markdown);

  enif_alloc_binary(ob->size, &output);
  memcpy(output.data, ob->data, ob->size);
  hoedown_buffer_free(ob);

  return enif_make_binary(env, &output);
}

static ERL_NIF_TERM
do_dirty_job(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  ERL_NIF_TERM result = do_job(env, argc, argv);
  return enif_schedule_dirty_nif_finalizer(env, result, enif_dirty_nif_finalizer);
}

static ERL_NIF_TERM
to_html(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
#ifdef ERL_NIF_DIRTY_SCHEDULER_SUPPORT
  if(is_short_job(env, argc, argv)) {
    return do_job(env, argc, argv);
  }
  else {
    return enif_schedule_dirty_nif(env, ERL_NIF_DIRTY_JOB_CPU_BOUND, do_dirty_job, argc, argv);
  }
#else
  return do_job(env, argc, argv);
#endif
}

static ErlNifFunc funcs[] = {
  { "to_html", 2, to_html }
};

ERL_NIF_INIT(Elixir.Markdown, funcs, NULL, NULL, NULL, NULL)
