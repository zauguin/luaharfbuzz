#include "luaharfbuzz.h"

static int face_new_from_blob(lua_State *L) {
  Face *f;
  Blob * blob = luaL_checkudata(L, 1, "harfbuzz.Blob");
  unsigned int font_index = (unsigned int) luaL_checkint(L, 2);

  f = (Face *)lua_newuserdata(L, sizeof(*f));
  luaL_getmetatable(L, "harfbuzz.Face");
  lua_setmetatable(L, -2);

  *f = hb_face_create(*blob, font_index);
  return 1;
}

static int face_destroy(lua_State *L) {
  Face *f;
  f = (Face *)luaL_checkudata(L, 1, "harfbuzz.Face");

  hb_face_destroy(*f);

  return 0;
}

static const struct luaL_Reg face_methods[] = {
	{"__gc", face_destroy },
  { NULL, NULL },
};

static const struct luaL_Reg face_functions[] = {
  { "new_from_blob", face_new_from_blob },
  { NULL,  NULL }
};

int register_face(lua_State *L) {
  luaL_newmetatable(L, "harfbuzz.Face");
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setfuncs(L, face_methods, 0);
  lua_pop(L,1);

  luaL_newlib(L, face_functions);
  return 1;
}

