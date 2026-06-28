import math
import sys



def func_value(poly, x):
    res = 0.0
    for coeff in poly:
        res = res * x + coeff
    return res

def get_derivative(poly):
    n = len(poly) - 1
    if n <= 0:
        return [0.0]
    return [poly[i] * (n - i) for i in range(n)]

def horner_method(a, root, power):
    # Понижение степени многочлена по схеме Горнера
    # инициализация массива новых коэффициентов
    b = [0.0] * power
    # первый коэффициент
    b[0] = a[0]
    # рекуррентная формула
    for i in range(1, power):
        b[i] = a[i] + root * b[i - 1]
    return b

def find_root_interval(poly, left, right, steps=10000):
    # поиск подотрезка внутри [l, r], где функция меняет знак
    # деление интервала на шаги
    step_size = (right - left) / steps
    x_curr = left
    f_curr = func_value(poly, x_curr)

    # проходим каждый шаг до выполнения f_curr * f_next <= 0
    for _ in range(steps):
        if abs(f_curr) < 1e-5:
            return x_curr - step_size, x_curr + step_size
        x_next = x_curr + step_size
        f_next = func_value(poly, x_next)
        if f_curr * f_next <= 0:
            return x_curr, x_next
        x_curr = x_next
        f_curr = f_next
    return None

def validate_input(prompt, type_func=float, condition=lambda x: True):
    while True:
        try:
            val = type_func(input(prompt))
            if condition(val):
                return val
            print("Ошибка: недопустимое значение.")
        except ValueError:
            print("Ошибка: введите число.")


# ─────────────────────────────────────────────
# Функции получения параметров для каждого метода
# ─────────────────────────────────────────────

def get_params_tangent():
    choice = input(
        "Использовать параметры по умолчанию (eps=1e-7, iter=10000, x0=0)? (да/нет): "
    ).lower().strip()
    if choice == "да":
        return 1e-7, 10000, 0.0
    eps      = validate_input("Введите eps: ")
    max_iter = validate_input("Введите итерации: ", int)
    x0       = validate_input("Введите начальное x0: ")
    return eps, max_iter, x0


def get_params_chord():
    choice = input(
        "Использовать параметры по умолчанию (x1=-100, x2=100, eps=1e-7, iter=10000)? (да/нет): "
    ).lower().strip()
    if choice == "да":
        return -100.0, 100.0, 1e-7, 10000
    l        = validate_input("x1: ")
    r        = validate_input("x2: ")
    eps      = validate_input("eps: ")
    max_iter = validate_input("итерации: ", int)
    return l, r, eps, max_iter


def get_params_combined():
    choice = input(
        "Использовать параметры по умолчанию (l=-100, r=100, eps=1e-7, iter=10000)? (да/нет): "
    ).lower().strip()
    if choice == "да":
        return -100.0, 100.0, 1e-7, 10000
    l        = validate_input("l: ")
    r        = validate_input("r: ")
    eps      = validate_input("eps: ")
    max_iter = validate_input("итерации: ", int)
    return l, r, eps, max_iter


def get_params_cardano():
    choice = input(
        "Использовать параметры по умолчанию (eps=1e-12)? (да/нет): "
    ).lower().strip()
    if choice == "да":
        return 1e-12
    eps = validate_input("Введите eps: ")
    return eps


def get_params_vieta():
    choice = input(
        "Использовать параметры по умолчанию (eps=1e-12)? (да/нет): "
    ).lower().strip()
    if choice == "да":
        return 1e-12
    eps = validate_input("Введите eps: ")
    return eps


# ─────────────────────────────────────────────
# Численные методы
# ─────────────────────────────────────────────

def kasat_method(poly):
    print("\n>>> Переход к методу КАСАТЕЛЬНЫХ")
    eps, max_iter, x = get_params_tangent()

    # перебор по количеству итераций
    df_poly = get_derivative(poly)

    # перебор по количеству итераций
    for _ in range(max_iter):

        # вычисление значения функции и производной
        f = func_value(poly, x)
        if abs(f) < eps:
            return x, True

        df = func_value(df_poly, x)

        # если значение производной точки равно нулю, смещаем x
        if abs(df) < eps:
            x += 0.1
            continue

        # x_new — это точка пересечения касательной, проведенной из x, с осью       абсцисс (OX).
        x_new = x - f / df

        # если разница между найденным x и данным x попадает в погрешность
        if abs(x_new - x) < eps and abs(f) < eps:
            return x_new, True

        x = x_new

    return x, False


def chord_method(poly):
    print("\n>>> Переход к методу ХОРД")
    l, r, eps, max_iter = get_params_chord()

    # берем интервал, удовлетворяющий условию f1 * f2 <= 0
    interval = find_root_interval(poly, l, r)
    if not interval:
        return None, False

    # устанавливаем границы
    x1, x2 = interval

    for _ in range(max_iter):
        f1 = func_value(poly, x1)
        f2 = func_value(poly, x2)
        if abs(f2 - f1) < eps:
            break

        # приближенное значение корня находится как точка пересечения хорды (прямой линии),
        # соединяющей точки (x1, f1) и (x2, f2), с осью OX.
        x_new = x1 - f1 * (x2 - x1) / (f2 - f1)
        f_new = func_value(poly, x_new)

        # проверяем чтобы значение было меньше заданной погрешности
        if abs(f_new) < eps:
            return x_new, True
        if f1 * f_new < 0:
            x2 = x_new
        else:
            x1 = x_new

    return None, False


def combined_method(poly):
    print("\n>>> КОМБИНИРОВАННЫЙ метод")
    l, r, eps, max_iter = get_params_combined()

    interval = find_root_interval(poly, l, r)
    if not interval:
        return None, False

    l, r = interval
    df_poly = get_derivative(poly)

    # переходим в перебор до максимального количества итераций
    for _ in range(max_iter):
        f_l = func_value(poly, l)
        f_r = func_value(poly, r)

        # защита от деления на ноль
        if abs(f_r - f_l) < eps:
            break

        # метод хорд
        x_hord = l - f_l * (r - l) / (f_r - f_l)
        f_hord = func_value(poly, x_hord)

        # метод касательных
        df_l = func_value(df_poly, l)
        if abs(df_l) < eps:
            x_kas = l + 0.1
        else:
            x_kas = l - f_l / df_l
        f_kas = func_value(poly, x_kas)

        # проверка точности
        if abs(f_hord) < eps:
            return x_hord, True
        if abs(f_kas) < eps:
            return x_kas, True

        # выбор лучшего приближения
        if abs(f_hord) <= abs(f_kas):
            x = x_hord
            f_x = f_hord
        else:
            x = x_kas
            f_x = f_kas

        # обновление интервала
        if f_l * f_x < 0:
            r = x
        else:
            l = x

        if abs(r - l) < eps:
            return x, True

    return None, False


# ─────────────────────────────────────────────
# Аналитические методы
# ─────────────────────────────────────────────

def cardano_solver(poly):
    print("\n>>> Применение метода КАРДАНО")
    eps = get_params_cardano()

    # распаковка коэффициентов кубического уравнения
    a, b, c, d = poly

    # приведение уравнения к пониженному виду x = y - b/(3a)
    # получаем уравнение вида: y^3 + p*y + q = 0
    p = (3 * a * c - b ** 2) / (3 * a ** 2)
    q = (2 * b ** 3 - 9 * a * b * c + 27 * a ** 2 * d) / (27 * a ** 3)

    # дискриминант кубического уравнения
    dis = (q / 2) ** 2 + (p / 3) ** 3

    # вычисление кубического корня
    def cbrt(v):
        return math.copysign(abs(v) ** (1 / 3), v)

    # список найденных корней
    roots = []
    shift = -b / (3 * a)

    # 1 СЛУЧАЙ: один действительный корень
    # дискриминант > 0
    if dis > eps:
        roots.append(
            cbrt(-q / 2 + math.sqrt(dis)) +
            cbrt(-q / 2 - math.sqrt(dis)) +
            shift
        )

    # 2 СЛУЧАЙ: два действительных корня
    # дискриминант = 0
    elif abs(dis) < eps:

        # первый корень
        f = cbrt(-q / 2)
        roots.extend([2 * f + shift, -f + shift])

    # 3 СЛУЧАЙ: три различных действительных корня
    # дискриминант < 0
    # используется тригонометрическая формула Кардано
    else:
        r = math.sqrt(-(p ** 3) / 27)
        phi = math.acos(max(-1.0, min(1.0, -q / (2 * r))))
        t = 2 * (r ** (1 / 3))
        for i in [0, 2, 4]:
            roots.append(t * math.cos((phi + i * math.pi) / 3) + shift)

    return roots


def vieta_solver(poly):
    print("\n>>> Применение метода ВИЕТА")
    eps = get_params_vieta()

    a, b, c = poly
    D = b ** 2 - 4 * a * c
    if D > 0:
        return [(-b + math.sqrt(D)) / (2 * a),
                (-b - math.sqrt(D)) / (2 * a)]
    if abs(D) < eps:
        return [-b / (2 * a)]
    return []


def main():
    print("РЕШЕНИЕ УРАВНЕНИЯ (ДО 6 СТЕПЕНИ)")

    # вводим коэффициенты с переводом в float
    flag = True
    coeffs = []
    while flag:
        try:
            coeffs = [float(input(f"Введите {n}: "))
                      for n in ['a', 'b', 'c', 'd', 'n', 'm', 'k']]
            flag = False
        except ValueError:
            print("Обязательно вводите число")

    if all(c == 0 for c in coeffs):
        print("\n" + "=" * 50)
        print("РЕЗУЛЬТАТ: x — любое число.")
        print("=" * 50)
        return

    # список, куда добавляются корни уравнения
    roots = []

    # копия введенных коэффициентов
    poly = coeffs
    while len(poly) > 1:

        while len(poly) > 1 and abs(poly[0]) == 0:
            poly.pop(0)

        deg = len(poly) - 1

        if deg <= 0:
            break

        found_root = None

        if deg >= 6:
            res, ok = kasat_method(poly)
            if ok:
                found_root = res

        elif deg == 5:
            res, ok = chord_method(poly)
            if ok:
                found_root = res

        elif deg == 4:
            res, ok = combined_method(poly)
            if ok:
                found_root = res

        elif deg == 3:
            roots.extend(cardano_solver(poly))
            break

        elif deg == 2:
            roots.extend(vieta_solver(poly))
            break

        elif deg == 1:
            found_root = -poly[1] / poly[0]

        if found_root is not None:
            found_root = round(found_root, 6)
            roots.append(found_root)
            power = len(poly) - 1
            poly = horner_method(poly, found_root, power)

        else:
            print("\n[!] Корень не найден.")
            break

    print("\n" + "=" * 50)

    if not roots:
        print("РЕЗУЛЬТАТ: Действительных корней нет.")
    else:
        print("ИТОГОВЫЕ КОРНИ:")
        final = sorted(list(set([round(r, 3) for r in roots])))
        for i, v in enumerate(final):
            print(f"x{i + 1} = {v:.3f}")

    print("=" * 50)


if __name__ == "__main__":
    main()
