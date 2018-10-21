#include "luaharfbuzz.h"

static int face_new(lua_State *L) {
  Face *f;
  hb_blob_t *blob;
  unsigned int font_index = 0;
  const char *file_name = luaL_checkstring(L, 1);

  if (lua_gettop(L) > 1)
    font_index = (unsigned int) luaL_checkinteger(L, 2);

  blob = hb_blob_create_from_file(file_name);

  f = (Face *)lua_newuserdata(L, sizeof(*f));
  luaL_getmetatable(L, "harfbuzz.Face");
  lua_setmetatable(L, -2);

  *f = hb_face_create(blob, font_index);
  return 1;
}

static int face_new_from_blob(lua_State *L) {
  Face *f;
  Blob *blob = luaL_checkudata(L, 1, "harfbuzz.Blob");
  unsigned int font_index = (unsigned int) luaL_checkinteger(L, 2);

  f = (Face *)lua_newuserdata(L, sizeof(*f));
  luaL_getmetatable(L, "harfbuzz.Face");
  lua_setmetatable(L, -2);

  *f = hb_face_create(*blob, font_index);
  return 1;
}

static void set_tags(lua_State *L, hb_tag_t *tags, unsigned int count) {
  unsigned int i;

  for (i = 0; i < count; i++) {
    lua_pushnumber(L, i + 1);

    Tag *tp = (Tag *)lua_newuserdata(L, sizeof(*tp));
    luaL_getmetatable(L, "harfbuzz.Tag");
    lua_setmetatable(L, -2);
    *tp = tags[i];

    lua_rawset(L, -3);
  }
}

static int face_get_table_tags(lua_State *L) {
  Face *f = (Face *)luaL_checkudata(L, 1, "harfbuzz.Face");
#define TABLE_SIZE 128
  unsigned int table_size = TABLE_SIZE, count;
  hb_tag_t tags[TABLE_SIZE];

  lua_newtable(L);

  count = hb_face_get_table_tags(*f, 0, &table_size, tags);
  if (count) {
    if (count <= table_size) {
      set_tags(L, tags, count);
    } else {
      hb_tag_t* tags = (hb_tag_t *) malloc(count * sizeof(hb_tag_t));
      hb_face_get_table_tags (*f, 0, &count, tags);
      set_tags(L, tags, count);
      free(tags);
    }
  }
#undef TABLE_SIZE

  return 1;
}

static int face_get_upem(lua_State *L) {
  Face *f = (Face *)luaL_checkudata(L, 1, "harfbuzz.Face");

  lua_pushinteger(L, hb_face_get_upem(*f));
  return 1;
}

static int face_destroy(lua_State *L) {
  Face *f = (Face *)luaL_checkudata(L, 1, "harfbuzz.Face");

  hb_face_destroy(*f);
  return 0;
}

static const struct luaL_Reg face_methods[] = {
  { "__gc", face_destroy },
  { "get_table_tags", face_get_table_tags },
  { "get_upem", face_get_upem },
  { NULL, NULL }
};

static const struct luaL_Reg face_functions[] = {
  { "new", face_new },
  { "new_from_blob", face_new_from_blob },
  { NULL,  NULL }
};

int register_face(lua_State *L) {
  return register_class(L, "harfbuzz.Face", face_methods, face_functions);
}
