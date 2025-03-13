import { getAllPosts } from '@/lib/blog'
import { BlogPostCard } from '@/components/blog-post-card'
import Header from '@/components/header'

export default function Home() {
  const posts = getAllPosts()

  return (
    <div className="min-h-screen bg-black text-gray-100">
      <main className="max-w-5xl mx-auto py-12 px-4">
        <Header />
        <div className="mt-12">
          <h1 className="text-4xl font-bold mb-8">Latest Posts</h1>
          <div className="grid gap-8 md:grid-cols-2">
            {posts.map((post) => (
              <BlogPostCard key={post.slug} post={post} />
            ))}
          </div>
        </div>
      </main>
    </div>
  )
}

