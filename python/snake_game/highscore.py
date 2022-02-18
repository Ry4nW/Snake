"""Persists the all-time high score to a small JSON file on disk."""
import json
import os


class HighScoreStore:
    def __init__(self, path):
        self.path = path
        self.value = self._load()

    def _load(self):
        if not os.path.exists(self.path):
            return 0
        try:
            with open(self.path, 'r') as f:
                return int(json.load(f).get('high_score', 0))
        except (json.JSONDecodeError, ValueError, OSError):
            return 0

    def update(self, score):
        if score <= self.value:
            return False
        self.value = score
        self._save()
        return True

    def _save(self):
        try:
            with open(self.path, 'w') as f:
                json.dump({'high_score': self.value}, f)
        except OSError:
            pass
