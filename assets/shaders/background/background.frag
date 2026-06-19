#version 330 core
out vec4 FragColor;

// 頂点シェーダーから来る箱のローカル座標
in vec3 localPos; 

// main.cppのループ内で送っている時間
uniform float uTime;
uniform float uEndFlash;

//光源と方向を定義
vec3 lightDir = normalize(vec3(0.5, 1.0, 1.0));

mat2 rot(float a) {
    float s = sin(a), c = cos(a);
    return mat2(c, -s, s, c);
}

float barDist(vec2 p, float width) {
    return length(max(abs(p) - width, 0.0));
}
float map(vec3 p) {

    p.xy *= rot(uTime * 0.1);

    // 空間をXYZの全方向にループさせる魔法の計算 (mod)
    vec3 q = mod(p, 6.0) - 3.0;

    float width = 0.3;
    float bar_x = barDist(q.yz, width);
    float bar_y = barDist(q.xz, width);
    float bar_z = barDist(q.xy, width);
    float grid = min(min(bar_x, bar_y), bar_z);

    vec3 q_cyl = mod(p, 0.3) - 0.15;

    float rad = 0.08;

    float cyl_x = length(q_cyl.yz) - rad;
    float cyl_y = length(q_cyl.xz) - rad;
    float cyl_z = length(q_cyl.xy) - rad;

    float cyl_grid = min(min(cyl_x, cyl_y), cyl_z);

    return max(grid, -cyl_grid);
}

vec3 calcNormal(vec3 p) {
    // e は微小なズレ（イプシロン）。Xに0.001、Yに0.0を入れる
    vec2 e = vec2(0.001, 0.0);

    // X, Y, Zそれぞれの方向にズラして距離を測り、ベクトルにして正規化
    return normalize(vec3(//
    map(p + e.xyy) - map(p - e.xyy), // X軸方向の傾き
    map(p + e.yxy) - map(p - e.yxy), // Y軸方向の傾き
    map(p + e.yyx) - map(p - e.yyx)  // Z軸方向の傾き
    ));
}

void main() {
    // ① レイの原点 (時間の経過とともにZ軸の奥へカメラが突き進む！)
    vec3 ro = vec3(0.0, 0.0, -uTime * 3.0);

    // レイの飛んでいく方向
    // 箱の中心から壁(localPos)に向かうベクトルを正規化すれば、そのまま全方位へのレイになります
    vec3 rd = normalize(localPos);

    // レイマーチングのメインループ (Sphere Tracing)
    float t = 0.0;     // レイが進んだ総距離
    float d = 0.0;     // オブジェクトまでの最短距離

    for(int i = 0; i < 128; i++) {
        vec3 p = ro + rd * t; // 現在のレイの先端位置
        d = map(p);           // そこから一番近い物体までの距離を測る

        // ぶつかった（距離がほぼ0になった）、または遠すぎたら探索終了
        if(d < 0.001 || t > 100.0)
            break;

        t += d; // ぶつからない安全な距離だけレイを前進させる
    }

    // ④ 描画（色塗り）
    vec3 col = vec3(1, 1, 1); // 基本の背景色（暗いネイビー）

    if(t < 100.0) {
        // ① ぶつかった正確な座標 (p) を再計算
        vec3 p = ro + rd * t;

        // ② 法線を計算（追加した関数を使用）
        vec3 n = calcNormal(p);

        // ④ 拡散反射（ディフューズ）の計算
        // 法線と光の方向の内積 (dot) を取る。0.0以下（裏側）は真っ黒にするために max() で制限
        float diff = max(dot(n, lightDir), 0.0);

        // ⑤ 環境光（アンビエント）
        // 影の部分が完全に真っ黒（0.0）にならないように、最低限の明るさを足す
        float ambient = 0.2;

        // 最終的な光の強さ（係数）
        float lighting = diff + ambient;

        // ⑥ 基本の色（サイバーな水色）に光の強さを掛ける
        vec3 baseColor = vec3(0.36, 0.35, 0.82);
        col = baseColor * lighting;

        float fog = 1.0 * (1.0 + t * t * 0.0005);
        col *= fog;
    }

    vec4 flash = vec4(1.0);

    FragColor = mix(vec4(col, 1.0), flash, uEndFlash);
}