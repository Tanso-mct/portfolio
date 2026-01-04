using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Constants : MonoBehaviour
{
    // Set FPS. Make sure that fps is this value.
    public const int SPECIFIED_FPS = 60;

    // Screen resolution
    public const int SCREEN_WIDTH = 1920;
    public const int SCREEN_HEIGHT = 1080;

    // Message processing parameters
    public const int MSG_NULL = 0;
    public const int MSG_SUCCESS = 1;
    public const int MSG_FAILED = 2;
    public const int MSG_ERROR = 3;
    public const int MSG_WARNING = 4;
    public const int MSG_CHANGE_SCENE = 5;
    public const int MSG_QUIT_GAME = 6;

    // Key bind
    public const  KeyCode KEY_ATTACK = KeyCode.Mouse0;
    public const  KeyCode KEY_DROP = KeyCode.Q;
    public const  KeyCode KEY_USE = KeyCode.Mouse1;

    public const  KeyCode KEY_HOT_BAR_1 = KeyCode.Alpha1;
    public const  KeyCode KEY_HOT_BAR_2 = KeyCode.Alpha2;
    public const  KeyCode KEY_HOT_BAR_3 = KeyCode.Alpha3;
    public const  KeyCode KEY_HOT_BAR_4 = KeyCode.Alpha4;
    public const  KeyCode KEY_HOT_BAR_5 = KeyCode.Alpha5;
    public const  KeyCode KEY_HOT_BAR_6 = KeyCode.Alpha6;
    public const  KeyCode KEY_HOT_BAR_7 = KeyCode.Alpha7;
    public const  KeyCode KEY_HOT_BAR_8 = KeyCode.Alpha8;
    public const  KeyCode KEY_HOT_BAR_9 = KeyCode.Alpha9;

    public const  KeyCode KEY_INVENTORY = KeyCode.E;

    public const  KeyCode KEY_JUMP = KeyCode.Space;
    public const  KeyCode KEY_SPRINT = KeyCode.LeftShift;
    public const  KeyCode KEY_LEFT = KeyCode.A;
    public const  KeyCode KEY_RIGHT = KeyCode.D;
    public const  KeyCode KEY_BACK = KeyCode.S;
    public const  KeyCode KEY_FORWARD = KeyCode.W;

    // Scene name
    public const string SCENE_MENU = "Menu";
    public const string SCENE_PLAY = "Play";

    // Element type
    public const string TYPE_IMAGE = "Image";
    public const string TYPE_INPUT_BOX = "InputBox";
    public const string TYPE_BUTTON = "Button";
    public const string TYPE_TEXT = "Text";
    public const string TYPE_SELECT_BAR = "SelectBar";

    // Control name
    public const string CONTROL_ATTACK = "Attack";
    public const string CONTROL_DROP_ITEM = "DropItem";
    public const string CONTROL_USE = "Use";

    public const string CONTROL_HS1 = "HS1";
    public const string CONTROL_HS2 = "HS2";
    public const string CONTROL_HS3 = "HS3";
    public const string CONTROL_HS4 = "HS4";
    public const string CONTROL_HS5 = "HS5";
    public const string CONTROL_HS6 = "HS6";
    public const string CONTROL_HS7 = "HS7";
    public const string CONTROL_HS8 = "HS8";
    public const string CONTROL_HS9 = "HS9";

    public const string CONTROL_INVENTORY = "Inventory";

    public const string CONTROL_JUMP = "Jump";
    public const string CONTROL_SPRINT = "Sprint";
    public const string CONTROL_LEFT = "Left";
    public const string CONTROL_RIGHT = "Right";
    public const string CONTROL_BACK = "Back";
    public const string CONTROL_FOR = "Forward";

    // Window name
    public const string WND_MENU = "WindowMenu";

    // Vaxel state
    public const int VAXEL_STATE_NULL = 0;
    public const int VAXEL_STATE_ITEM = 1;
    public const int VAXEL_STATE_BLOCK = 2;
    public const int VAXEL_STATE_ENTITY = 3;

    // Vaxel type
    public const int BLOCK_TYPE_CANT_SET = -1;
    public const int BLOCK_TYPE_CANT_BREAK = -1;
    public const int FRAME_BLOCK_IS_NULL = -2;

    public enum VAXEL_TYPE
    {
        AIR,
        BEDROCK,
        DIRT,
        GRASS_TOP, GRASS_SIDE, GRASS_BOTTOM,
        WATER, LAVA, OBSIDIAN,
        STONE, COBBLESTONE, STONE_ANDESITE, STONE_DIORITE, STONE_GRANITE,
        COAL_ORE, IRON_ORE, GOLD_ORE, DIAMOND_ORE, EMERALD_ORE, LAPIS_ORE,
        LEAVES, 
        LOG_OAK_TOP, LOG_OAK, LOG_OAK_BOTTOM, 
        PLANKS_OAK, PLANKS_BIRCH,
        LOG_BIRCH_TOP, LOG_BIRCH, LOG_BIRCH_BOTTOM,
        GRAVEL, 
        HARDENED_CLAY,
        HARDENED_CLAY_STAINED_BLACK,
        HARDENED_CLAY_STAINED_BLUE,
        HARDENED_CLAY_STAINED_BROWN,
        HARDENED_CLAY_STAINED_CYAN,
        HARDENED_CLAY_STAINED_GRAY,
        HARDENED_CLAY_STAINED_GREEN,
        HARDENED_CLAY_STAINED_LIGHT_BLUE,
        HARDENED_CLAY_STAINED_LIME,
        HARDENED_CLAY_STAINED_MAGENTA,
        HARDENED_CLAY_STAINED_ORANGE,
        HARDENED_CLAY_STAINED_PINK,
        HARDENED_CLAY_STAINED_PURPLE,
        HARDENED_CLAY_STAINED_RED,
        HARDENED_CLAY_STAINED_SILVER,
        HARDENED_CLAY_STAINED_WHITE,
        HARDENED_CLAY_STAINED_YELLOW,
        WOOL_COLORED_BLUE,
        WOOL_COLORED_BROWN,
        WOOL_COLORED_CYAN,
        WOOL_COLORED_GRAY,
        WOOL_COLORED_GREEN,
        WOOL_COLORED_LIGHT_BLUE,
        WOOL_COLORED_LIME,
        WOOL_COLORED_MAGENTA,
        WOOL_COLORED_ORANGE,
        WOOL_COLORED_PINK,
        WOOL_COLORED_PURPLE,
        WOOL_COLORED_RED,
        WOOL_COLORED_SILVER,
        WOOL_COLORED_WHITE,
        WOOL_COLORED_YELLOW,

    }

    static public List<string> VAXEL_SPRITE_PATH = new List<string>
    {
        SPRITE_NULL,
        SPRITE_BEDROCK,
        SPRITE_DIRT,
        SPRITE_GRASS_TOP, SPRITE_GRASS_SIDE, SPRITE_GRASS_BOTTOM,
        SPRITE_WATER, SPRITE_LAVA, SPRITE_OBSIDIAN,
        SPRITE_STONE, SPRITE_COBBLESTONE, SPRITE_STONE_ANDESITE, SPRITE_STONE_DIORITE, SPRITE_STONE_GRANITE,
        SPRITE_COAL_ORE, SPRITE_IRON_ORE, SPRITE_GOLD_ORE, SPRITE_DIAMOND_ORE, SPRITE_EMERALD_ORE, SPRITE_LAPIS_ORE,
        SPRITE_LEAVES,
        SPRITE_LOG_OAK_TOP, SPRITE_LOG_OAK, SPRITE_LOG_OAK_TOP,
        SPRITE_PLANKS_OAK, SPRITE_PLANKS_BIRCH,
        SPRITE_LOG_BIRCH_TOP, SPRITE_LOG_BIRCH, SPRITE_LOG_BIRCH_TOP,
        SPRITE_GRAVEL,
        SPRITE_HARDENED_CLAY,
        SPRITE_HARDENED_CLAY_STAINED_BLACK,
        SPRITE_HARDENED_CLAY_STAINED_BLUE,
        SPRITE_HARDENED_CLAY_STAINED_BROWN,
        SPRITE_HARDENED_CLAY_STAINED_CYAN,
        SPRITE_HARDENED_CLAY_STAINED_GRAY,
        SPRITE_HARDENED_CLAY_STAINED_GREEN,
        SPRITE_HARDENED_CLAY_STAINED_LIGHT_BLUE,
        SPRITE_HARDENED_CLAY_STAINED_LIME,
        SPRITE_HARDENED_CLAY_STAINED_MAGENTA,
        SPRITE_HARDENED_CLAY_STAINED_ORANGE,
        SPRITE_HARDENED_CLAY_STAINED_PINK,
        SPRITE_HARDENED_CLAY_STAINED_PURPLE,
        SPRITE_HARDENED_CLAY_STAINED_RED,
        SPRITE_HARDENED_CLAY_STAINED_SILVER,
        SPRITE_HARDENED_CLAY_STAINED_WHITE,
        SPRITE_HARDENED_CLAY_STAINED_YELLOW,
        SPRITE_WOOL_COLORED_BLUE,
        SPRITE_WOOL_COLORED_BROWN,
        SPRITE_WOOL_COLORED_CYAN,
        SPRITE_WOOL_COLORED_GRAY,
        SPRITE_WOOL_COLORED_GREEN,
        SPRITE_WOOL_COLORED_LIGHT_BLUE,
        SPRITE_WOOL_COLORED_LIME,
        SPRITE_WOOL_COLORED_MAGENTA,
        SPRITE_WOOL_COLORED_ORANGE,
        SPRITE_WOOL_COLORED_PINK,
        SPRITE_WOOL_COLORED_PURPLE,
        SPRITE_WOOL_COLORED_RED,
        SPRITE_WOOL_COLORED_SILVER,
        SPRITE_WOOL_COLORED_WHITE,
        SPRITE_WOOL_COLORED_YELLOW
    };

    public const int TARGET_BLOCK_SELECT = 0;
    public const int TARGET_BLOCK_SET = 1;

    // Mining Speed
    public const float MINING_SPEED_HAND = 1.0f;

    // Block object type    
    public const string BLOCK_OBJ_FRONT = "Front";
    public const string BLOCK_OBJ_BACK = "Back";
    public const string BLOCK_OBJ_LEFT = "Left";
    public const string BLOCK_OBJ_RIGHT = "Right";
    public const string BLOCK_OBJ_TOP = "Top";
    public const string BLOCK_OBJ_BOTTOM = "Bottom";

    public const int CHUCK_SIZE = 16;
    

    // Texture face
    public const int TEXTURE_FACE_FULL = 0;
    public const int TEXTURE_FACE_TOP = 0;
    public const int TEXTURE_FACE_SIDE = 2;
    public const int TEXTURE_FACE_BOTTOM = 1;
    public const int TEXTURE_FACE_FRONT = 2;
    public const int TEXTURE_FACE_BACK = 3;
    public const int TEXTURE_FACE_LEFT = 4;
    public const int TEXTURE_FACE_RIGHT = 5;

    // Texture atlas path
    public const string TEXTURE_ATLAS_BLOCK = "Textures/Atlas/blocks";

    // Sprite path
    public const string SPRITE_BEDROCK = "Textures/blocks/bedrock";
    public const string SPRITE_DIRT = "Textures/blocks/dirt";
    public const string SPRITE_GRASS_TOP = "Textures/blocks/grass_top";
    public const string SPRITE_GRASS_SIDE = "Textures/blocks/grass_side";
    public const string SPRITE_GRASS_BOTTOM = "Textures/blocks/dirt";

    public const string SPRITE_OBSIDIAN = "Textures/blocks/obsidian";

    public const string SPRITE_WATER = "Textures/blocks/water";
    public const string SPRITE_LAVA = "Textures/blocks/lava";

    public const string SPRITE_STONE = "Textures/blocks/stone";
    public const string SPRITE_COBBLESTONE = "Textures/blocks/cobblestone";
    public const string SPRITE_STONE_ANDESITE = "Textures/blocks/stone_andesite";
    public const string SPRITE_STONE_DIORITE = "Textures/blocks/stone_diorite";
    public const string SPRITE_STONE_GRANITE = "Textures/blocks/stone_granite";
    public const string SPRITE_COAL_ORE = "Textures/blocks/coal_ore";
    public const string SPRITE_IRON_ORE = "Textures/blocks/iron_ore";
    public const string SPRITE_GOLD_ORE = "Textures/blocks/gold_ore";
    public const string SPRITE_DIAMOND_ORE = "Textures/blocks/diamond_ore";
    public const string SPRITE_EMERALD_ORE = "Textures/blocks/emerald_ore";
    public const string SPRITE_LAPIS_ORE = "Textures/blocks/lapis_ore";

    public const string SPRITE_LEAVES = "Textures/blocks/leaves";
    public const string SPRITE_LOG_OAK_TOP = "Textures/blocks/log_oak_top";
    public const string SPRITE_LOG_OAK = "Textures/blocks/log_oak";
    public const string SPRITE_PLANKS_OAK = "Textures/blocks/planks_oak";
    public const string SPRITE_PLANKS_BIRCH = "Textures/blocks/planks_birch";
    public const string SPRITE_LOG_BIRCH_TOP = "Textures/blocks/log_birch_top";
    public const string SPRITE_LOG_BIRCH = "Textures/blocks/log_birch";

    public const string SPRITE_GRAVEL = "Textures/blocks/gravel";

    public const string SPRITE_HARDENED_CLAY = "Textures/blocks/hardened_clay";
    public const string SPRITE_HARDENED_CLAY_STAINED_BLACK = "Textures/blocks/hardened_clay_stained_black";
    public const string SPRITE_HARDENED_CLAY_STAINED_BLUE = "Textures/blocks/hardened_clay_stained_blue";
    public const string SPRITE_HARDENED_CLAY_STAINED_BROWN = "Textures/blocks/hardened_clay_stained_brown";
    public const string SPRITE_HARDENED_CLAY_STAINED_CYAN = "Textures/blocks/hardened_clay_stained_cyan";
    public const string SPRITE_HARDENED_CLAY_STAINED_GRAY = "Textures/blocks/hardened_clay_stained_gray";
    public const string SPRITE_HARDENED_CLAY_STAINED_GREEN = "Textures/blocks/hardened_clay_stained_green";
    public const string SPRITE_HARDENED_CLAY_STAINED_LIGHT_BLUE = "Textures/blocks/hardened_clay_stained_light_blue";
    public const string SPRITE_HARDENED_CLAY_STAINED_LIME = "Textures/blocks/hardened_clay_stained_lime";
    public const string SPRITE_HARDENED_CLAY_STAINED_MAGENTA = "Textures/blocks/hardened_clay_stained_magenta";
    public const string SPRITE_HARDENED_CLAY_STAINED_ORANGE = "Textures/blocks/hardened_clay_stained_orange";
    public const string SPRITE_HARDENED_CLAY_STAINED_PINK = "Textures/blocks/hardened_clay_stained_pink";
    public const string SPRITE_HARDENED_CLAY_STAINED_PURPLE = "Textures/blocks/hardened_clay_stained_purple";
    public const string SPRITE_HARDENED_CLAY_STAINED_RED = "Textures/blocks/hardened_clay_stained_red";
    public const string SPRITE_HARDENED_CLAY_STAINED_SILVER = "Textures/blocks/hardened_clay_stained_silver";
    public const string SPRITE_HARDENED_CLAY_STAINED_WHITE = "Textures/blocks/hardened_clay_stained_white";
    public const string SPRITE_HARDENED_CLAY_STAINED_YELLOW = "Textures/blocks/hardened_clay_stained_yellow";

    public const string SPRITE_WOOL_COLORED_BLUE = "Textures/blocks/wool_colored_blue";
    public const string SPRITE_WOOL_COLORED_BROWN = "Textures/blocks/wool_colored_brown";
    public const string SPRITE_WOOL_COLORED_CYAN = "Textures/blocks/wool_colored_cyan";
    public const string SPRITE_WOOL_COLORED_GRAY = "Textures/blocks/wool_colored_gray";
    public const string SPRITE_WOOL_COLORED_GREEN = "Textures/blocks/wool_colored_green";
    public const string SPRITE_WOOL_COLORED_LIGHT_BLUE = "Textures/blocks/wool_colored_light_blue";
    public const string SPRITE_WOOL_COLORED_LIME = "Textures/blocks/wool_colored_lime";
    public const string SPRITE_WOOL_COLORED_MAGENTA = "Textures/blocks/wool_colored_magenta";
    public const string SPRITE_WOOL_COLORED_ORANGE = "Textures/blocks/wool_colored_orange";
    public const string SPRITE_WOOL_COLORED_PINK = "Textures/blocks/wool_colored_pink";
    public const string SPRITE_WOOL_COLORED_PURPLE = "Textures/blocks/wool_colored_purple";
    public const string SPRITE_WOOL_COLORED_RED = "Textures/blocks/wool_colored_red";
    public const string SPRITE_WOOL_COLORED_SILVER = "Textures/blocks/wool_colored_silver";
    public const string SPRITE_WOOL_COLORED_WHITE = "Textures/blocks/wool_colored_white";
    public const string SPRITE_WOOL_COLORED_YELLOW = "Textures/blocks/wool_colored_yellow";

    public const string SPRITE_NULL = "Textures/null_content";

    // World Prefab path
    public const string PREFAB_VAXEL = "World/Vaxel";
    public const string PREFAB_BLOCK = "World/Block";
    public const string PREFAB_ENTITY_BLOCK = "World/EntityBlock";

    // World size
    public const int WORLD_SIZE = 1025;
    public const int WORLD_HALF_SIZE = 512;
    public const int WORLD_HEIGHT = 320;


    // World Info
    public const string WORLD_MODE_SURVIVAL = "Survival";
    public const string WORLD_TYPE_NORMAL = "Normal";
    public const string WORLD_TYPE_FLAT = "Flat";


    // Animation
    public const string ANIM_TYPE = "AnimType";
    public const int ANIM_IDLE = 0;
    public const int ANIM_PLAYER_BREATH = 1;
    public const int ANIM_PLAYER_WALK = 2;
    public const int ANIM_PLAYER_RUN = 3;
    public const int ANIM_PLAYER_USE = 4;

    public const string ANIM_STATE_USE_CANVAS = "use_canvas";

    // Unity Tags
    public const string TAG_PLAYER = "Player";
    public const string TAG_BLOCK_TOP = "Block_Top";
    public const string TAG_ENTITY = "Entity";


    // ソースメッシュの頂点数、UV数、頂点インデックス数のうち最大のもの
    public static int SOURCE_MESH_VS_MAX = 0;
    public static int SOURCE_MESH_TRIS_MAX = 0;

    public static void SetShaderConstants(ref ComputeShader shader)
    {
        shader.SetInt("CHUCK_SIZE", CHUCK_SIZE);
        shader.SetInt("WORLD_SIZE", WORLD_SIZE);
        shader.SetInt("WORLD_HALF_SIZE", WORLD_HALF_SIZE);
        shader.SetInt("WORLD_HEIGHT", WORLD_HEIGHT);
    }

    // Sound
    public const string SOUND_HAL = "hal";
    public const string SOUND_MENU = "menu";

    public const string SOUND_DIG_CLOTH = "dig_cloth";
    public const string SOUND_DIG_GRASS = "dig_grass";
    public const string SOUND_DIG_STONE = "dig_stone";
    public const string SOUND_DIG_WOOD = "dig_wood";
    public const string SOUND_DIG_GRAVEL = "dig_gravel";
    public const string SOUND_DIG_SAND = "dig_sand";

    public const string SOUND_CLICK = "click";

    public const string SOUND_FALL_SMALL = "fall_small";


}



