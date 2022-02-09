import argparse

from snake_game.game import Game


def parse_args():
    parser = argparse.ArgumentParser(description='Snake')
    parser.add_argument(
        '--ai', action='store_true',
        help='start the round with the AI autopilot enabled instead of waiting for keyboard input',
    )
    return parser.parse_args()


def main():
    args = parse_args()
    Game(start_with_ai=args.ai).run()


if __name__ == '__main__':
    main()
