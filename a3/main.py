from collections import defaultdict
from abc import ABC, abstractmethod

# ---------- Iterator Interfaces and Implementations ----------

class SocialNetwork(ABC):
    """Abstract base class representing a social network."""
    @abstractmethod
    def create_friends_iterator(self, profile_id):
        pass

class ProfileIterator(ABC):
    """Abstract base class for profile iterators."""
    def __iter__(self):
        return self
    
    @abstractmethod
    def __next__(self):
        pass

# ---------- Concrete Collection and Iterator ----------

class SocialMedia(SocialNetwork):
    """Concrete social network class."""
    def __init__(self):
        self.friend_graph = defaultdict(list)  # Adjacency list representation
        self.profiles = {}  # Dictionary to store profiles by ID

    def add_profile(self, profile_id, email):
        """Adds a new profile to the social network."""
        if profile_id not in self.profiles:
            self.profiles[profile_id] = Profile(profile_id, email)

    def add_friendship(self, profile_id, friend_id):
        """Creates a bidirectional friendship between two profiles."""
        if profile_id in self.profiles and friend_id in self.profiles:
            self.friend_graph[profile_id].append(self.profiles[friend_id])
            self.friend_graph[friend_id].append(self.profiles[profile_id])

    def get_friends(self, profile_id):
        """Returns a list of friends for the given profile ID."""
        return self.friend_graph.get(profile_id, [])

    def create_friends_iterator(self, profile_id):
        """Creates an iterator for the friends of a given profile."""
        return FriendIterator(self, profile_id)

class FriendIterator(ProfileIterator):
    """Iterator for traversing friends of a profile."""
    def __init__(self, social_media, profile_id):
        self.social_media = social_media
        self.profile_id = profile_id
        self.current_index = 0
        self.friends = self.social_media.get_friends(self.profile_id)

    def __next__(self):
        """Returns the next friend in the iteration."""
        if self.current_index < len(self.friends):
            friend = self.friends[self.current_index]
            self.current_index += 1
            return friend
        else:
            raise StopIteration()

# ---------- Profile Class ----------

class Profile:
    """Class representing a user profile."""
    def __init__(self, profile_id, email):
        self.profile_id = profile_id
        self.email = email

    def get_id(self):
        """Returns the profile ID."""
        return self.profile_id

    def get_email(self):
        """Returns the email of the profile."""
        return self.email

# ---------- Client Code ----------

class SocialSpammer:
    """Client class that uses an iterator to send messages."""
    def send(self, iterator, message):
        """Sends a message to all profiles in the iterator."""
        for profile in iterator:
            self.send_email(profile.get_email(), message)

    def send_email(self, email, message):
        """Simulates sending an email (for demonstration purposes)."""
        print(f"Sending email to {email}: {message}")

# ---------- Application ----------

class Application:
    """Application that configures and uses the social network."""
    def __init__(self):
        self.network = SocialMedia()
        self.spammer = SocialSpammer()
        self.setup_network()

    def setup_network(self):
        """Sets up the social network with profiles and friendships."""
        # Adding profiles
        self.network.add_profile("1", "user1@example.com")
        self.network.add_profile("2", "user2@example.com")
        self.network.add_profile("3", "user3@example.com")

        # Establishing friendships
        self.network.add_friendship("1", "2")
        self.network.add_friendship("1", "3")

    def send_spam_to_friends(self, profile_id):
        """Sends spam messages to friends of the given profile."""
        iterator = self.network.create_friends_iterator(profile_id)
        self.spammer.send(iterator, "Buy our new product!")

# ---------- Usage Example ----------

if __name__ == "__main__":
    app = Application()
    app.send_spam_to_friends("1")  # Sends spam to user 1's friends
