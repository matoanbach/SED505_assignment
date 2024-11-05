from collections import defaultdict
from abc import ABC, abstractmethod

'''
    Iterator declaration area start from here
'''

# Define an abstract base class for the social network
class SocialNetwork(ABC):
    # Abstract method to create a friends iterator for a given profile ID
    @abstractmethod
    def createFriendsIterator(self, profileId):
        pass

# Concrete implementation of the SocialNetwork interface
class SocialMedia(SocialNetwork):
    def __init__(self):
        # Initialize a default dictionary to represent the friend graph
        # Key: profile ID, Value: list of Profile objects representing friends
        self.friendGraph = defaultdict(list)
        # Initialize a default dictionary to store profiles
        # Key: profile ID, Value: Profile object
        self.profiles = defaultdict(Profile)
        
    def setProfile(self, personId, personEmail):
        # Add a new profile to the network if it doesn't already exist
        if personId not in self.profiles:
            self.profiles[personId] = Profile(personId, personEmail)
                
    def setFriend(self, personId, hisFriendId):
        # Establish a friendship between two profiles if both exist
        if personId in self.profiles and hisFriendId in self.profiles:
            # Append the friend's Profile object to the person's friend list
            self.friendGraph[personId].append(self.profiles[hisFriendId])
    
    def socialListRequest(self, profileId):
        # Retrieve the list of friends for the given profile ID
        return self.friendGraph[profileId] if profileId in self.friendGraph else []
    
    def createFriendsIterator(self, profileId):
        # Create and return an iterator over the friends of the given profile ID
        return SocialMediaIterator(self, profileId)
    
# Define an abstract base class for profile iterators
class ProfileIterator(ABC):
    # Abstract method to get the next profile in the iteration
    @abstractmethod
    def getNext(self):
        pass
    
    # Abstract method to check if more profiles are available
    @abstractmethod
    def hasMore(self):
        pass

# Concrete implementation of the ProfileIterator interface
class SocialMediaIterator(ProfileIterator):
    def __init__(self, socialMedia: SocialMedia, profileId):
        # Reference to the SocialMedia network
        self.socialMedia = socialMedia
        # The profile ID whose friends we are iterating over
        self.profileId = profileId
        # Current position in the iteration
        self.currentPosition = 0
        # Cache to store the list of friends
        self.cache = []
        # Initialize the cache with the friends list
        self.initCache()
            
    def initCache(self):
        # Fetch the friends list from the social media network
        self.cache = self.socialMedia.socialListRequest(self.profileId)
            
    def getNext(self):
        # Return the next Profile object if available
        if self.hasMore():
            result = self.cache[self.currentPosition]
            # Move to the next position
            self.currentPosition += 1
            return result
        # Return None if no more profiles are available
        return None
    
    def hasMore(self):
        # Check if there are more profiles to iterate over
        return self.currentPosition < len(self.cache)
    
# Class representing a user profile
class Profile:
    def __init__(self, id, email):
        # Initialize the profile with an ID and email
        self.id = id
        self.email = email

    def getId(self):
        # Return the profile ID
        return self.id
    
    def getEmail(self):
        # Return the profile email
        return self.email


'''
    Client area will start from here
'''

# Client class that uses the iterator to retrieve profile information
class SocialGetter: 
    def get(self, iterator: SocialMediaIterator) -> list:
        # List to store tuples of (profile ID, email)
        info_list = []
        # Iterate over the profiles using the iterator
        while iterator.hasMore():
            profile = iterator.getNext()
            # Append the profile's ID and email to the list
            info_list.append((profile.getId(), profile.getEmail()))
        # Return the collected profile information
        return info_list

# Application class that sets up the network and retrieves friends' information
class Application:
    def __init__(self):
        # Initialize the SocialGetter instance
        self.getter = SocialGetter()
        # Initialize the SocialMedia network
        self.network = SocialMedia()
    
    def getFriendInfo(self, profile: Profile):
        # Create an iterator for the given profile's friends
        iterator = self.network.createFriendsIterator(profile.getId())
        # Use the getter to retrieve and return the friends' information
        return self.getter.get(iterator)

if __name__ == "__main__":
    # Example usage
    app = Application()
    # Add profiles to the network
    app.network.setProfile("1", "profile1@example.com")
    app.network.setProfile("2", "profile2@example.com")
    app.network.setProfile("3", "profile3@example.com")
    # Establish friend relationships
    app.network.setFriend("1", "2")  # Adding friend relationship
    app.network.setFriend("1", "3")  # Adding another friend

    # Retrieve the Profile object for profile ID "1"
    profile1 = app.network.profiles["1"]

    # Retrieve and print the friends' information for profile "1"
    print(app.getFriendInfo(profile1))  # Should return [('2', 'profile2@example.com'), ('3', 'profile3@example.com')]
